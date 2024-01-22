//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.6
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 1.8.9
// ----------------------------------------------------------------------------
// SubframeSelectorCache.cpp - Released 2024-01-19T15:23:40Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2017-2021 Cameron Leger
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

#include "SubframeSelectorCache.h"

#include <pcl/Settings.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SubframeSelectorCache* TheSubframeSelectorCache = nullptr;

// ----------------------------------------------------------------------------

void SubframeSelectorCacheItem::AssignData( const FileDataCacheItem& item )
{
#define src static_cast<const SubframeSelectorCacheItem&>( item )
   fwhm                  = src.fwhm;
   fwhmMeanDev           = src.fwhmMeanDev;
   eccentricity          = src.eccentricity;
   eccentricityMeanDev   = src.eccentricityMeanDev;
   psfSignalWeight       = src.psfSignalWeight;
   psfSNR                = src.psfSNR;
   psfScale              = src.psfScale;
   psfScaleSNR           = src.psfScaleSNR;
   psfFlux               = src.psfFlux;
   psfFluxPower          = src.psfFluxPower;
   psfTotalMeanFlux      = src.psfTotalMeanFlux;
   psfTotalMeanPowerFlux = src.psfTotalMeanPowerFlux;
   psfCount              = src.psfCount;
   MStar                 = src.MStar;
   NStar                 = src.NStar;
   snrWeight             = src.snrWeight;
   median                = src.median;
   medianMeanDev         = src.medianMeanDev;
   noise                 = src.noise;
   noiseRatio            = src.noiseRatio;
   stars                 = src.stars;
   starResidual          = src.starResidual;
   starResidualMeanDev   = src.starResidualMeanDev;
   azimuth               = src.azimuth;
   altitude              = src.altitude;
#undef src
}

// ----------------------------------------------------------------------------

IsoStringList SubframeSelectorCacheItem::SerializedData() const
{
   IsoStringList tokens;
   tokens << "fwhm"                  << IsoString().Format( "%.4f", fwhm )
          << "fwhmMeanDev"           << IsoString().Format( "%.4e", fwhmMeanDev )
          << "eccentricity"          << IsoString().Format( "%.4f", eccentricity )
          << "eccentricityMeanDev"   << IsoString().Format( "%.4e", eccentricityMeanDev )
          << "psfSignalWeight"       << IsoString().Format( "%.4e", psfSignalWeight )
          << "psfSNR"                << IsoString().Format( "%.4e", psfSNR )
          << "psfScale"              << IsoString().Format( "%.4e", psfScale )
          << "psfScaleSNR"           << IsoString().Format( "%.4e", psfScaleSNR )
          << "psfFlux"               << IsoString().Format( "%.4e", psfFlux )
          << "psfFluxPower"          << IsoString().Format( "%.4e", psfFluxPower )
          << "psfTotalMeanFlux"      << IsoString().Format( "%.4e", psfTotalMeanFlux )
          << "psfTotalMeanPowerFlux" << IsoString().Format( "%.4e", psfTotalMeanPowerFlux )
          << "psfCount"              << IsoString().Format( "%u", psfCount )
          << "MStar"                 << IsoString().Format( "%.4e", MStar )
          << "NStar"                 << IsoString().Format( "%.4e", NStar )
          << "snrWeight"             << IsoString().Format( "%.4e", snrWeight )
          << "median"                << IsoString().Format( "%.4e", median )
          << "medianMeanDev"         << IsoString().Format( "%.4e", medianMeanDev )
          << "noise"                 << IsoString().Format( "%.4e", noise )
          << "noiseRatio"            << IsoString().Format( "%.4f", noiseRatio )
          << "stars"                 << IsoString().Format( "%u", stars )
          << "starResidual"          << IsoString().Format( "%.4e", starResidual )
          << "starResidualMeanDev"   << IsoString().Format( "%.4e", starResidualMeanDev )
          << "azimuth"               << IsoString().Format( "%.4f", azimuth )
          << "altitude"              << IsoString().Format( "%.4f", altitude );
   return tokens;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorCacheItem::DeserializeData( const IsoStringList& tokens )
{
   AssignData( SubframeSelectorCacheItem() );

   for ( IsoStringList::const_iterator i = tokens.Begin(); i != tokens.End(); )
      if ( *i == "fwhm" )
      {
         if ( !(++i)->TryToDouble( fwhm ) )
            return false;
      }
      else if ( *i == "fwhmMeanDev" )
      {
         if ( !(++i)->TryToDouble( fwhmMeanDev ) )
            return false;
      }
      else if ( *i == "eccentricity" )
      {
         if ( !(++i)->TryToDouble( eccentricity ) )
            return false;
      }
      else if ( *i == "eccentricityMeanDev" )
      {
         if ( !(++i)->TryToDouble( eccentricityMeanDev ) )
            return false;
      }
      else if ( *i == "psfSignalWeight" )
      {
         if ( !(++i)->TryToDouble( psfSignalWeight ) )
            return false;
      }
      else if ( *i == "psfSNR" )
      {
         if ( !(++i)->TryToDouble( psfSNR ) )
            return false;
      }
      else if ( *i == "psfScale" )
      {
         if ( !(++i)->TryToDouble( psfScale ) )
            return false;
      }
      else if ( *i == "psfScaleSNR" )
      {
         if ( !(++i)->TryToDouble( psfScaleSNR ) )
            return false;
      }
      else if ( *i == "psfFlux" )
      {
         if ( !(++i)->TryToDouble( psfFlux ) )
            return false;
      }
      else if ( *i == "psfFluxPower" )
      {
         if ( !(++i)->TryToDouble( psfFluxPower ) )
            return false;
      }
      else if ( *i == "psfTotalMeanFlux" )
      {
         if ( !(++i)->TryToDouble( psfTotalMeanFlux ) )
            return false;
      }
      else if ( *i == "psfTotalMeanPowerFlux" )
      {
         if ( !(++i)->TryToDouble( psfTotalMeanPowerFlux ) )
            return false;
      }
      else if ( *i == "psfCount" )
      {
         if ( !(++i)->TryToUInt( psfCount ) )
            return false;
      }
      else if ( *i == "MStar" )
      {
         if ( !(++i)->TryToDouble( MStar ) )
            return false;
      }
      else if ( *i == "NStar" )
      {
         if ( !(++i)->TryToDouble( NStar ) )
            return false;
      }
      else if ( *i == "snrWeight" )
      {
         if ( !(++i)->TryToDouble( snrWeight ) )
            return false;
      }
      else if ( *i == "median" )
      {
         if ( !(++i)->TryToDouble( median ) )
            return false;
      }
      else if ( *i == "medianMeanDev" )
      {
         if ( !(++i)->TryToDouble( medianMeanDev ) )
            return false;
      }
      else if ( *i == "noise" )
      {
         if ( !(++i)->TryToDouble( noise ) )
            return false;
      }
      else if ( *i == "noiseRatio" )
      {
         if ( !(++i)->TryToDouble( noiseRatio ) )
            return false;
      }
      else if ( *i == "stars" )
      {
         if ( !(++i)->TryToUInt( stars ) )
            return false;
      }
      else if ( *i == "starResidual" )
      {
         if ( !(++i)->TryToDouble( starResidual ) )
            return false;
      }
      else if ( *i == "starResidualMeanDev" )
      {
         if ( !(++i)->TryToDouble( starResidualMeanDev ) )
            return false;
      }
      else if ( *i == "azimuth" )
      {
         if ( !(++i)->TryToDouble( azimuth ) )
            return false;
      }
      else if ( *i == "altitude" )
      {
         if ( !(++i)->TryToDouble( altitude ) )
            return false;
      }
      else
      {
         ++i;
      }

   return true;
}

// ----------------------------------------------------------------------------

SubframeSelectorCache::SubframeSelectorCache()
   : FileDataCache( "SubframeSelector" )
{
   if ( TheSubframeSelectorCache == nullptr )
      TheSubframeSelectorCache = this;

   int maxItemDuration = 30;
   Settings::Read( "/Cache/MaxItemDuration", maxItemDuration );
   SetMaxItemDuration( maxItemDuration );
   Settings::Read( "/Cache/Persistent", m_persistent );
}

// ----------------------------------------------------------------------------

SubframeSelectorCache::~SubframeSelectorCache()
{
   if ( TheSubframeSelectorCache == this )
      TheSubframeSelectorCache = nullptr;
}

// ----------------------------------------------------------------------------

void SubframeSelectorCache::SetPersistent( bool persistent )
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

void SubframeSelectorCache::SetMaxItemDuration( int days )
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
// EOF SubframeSelectorCache.cpp - Released 2024-01-19T15:23:40Z
