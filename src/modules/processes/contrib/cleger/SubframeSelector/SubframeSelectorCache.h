//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.3
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 1.9.1
// ----------------------------------------------------------------------------
// SubframeSelectorCache.h - Released 2024-12-11T17:43:17Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2017-2021 Cameron Leger
// Copyright (c) 2020-2024 Juan Conejero, PTeam
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

#ifndef __SubframeSelectorCache_h
#define __SubframeSelectorCache_h

#include <pcl/FileDataCache.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class SubframeSelectorCacheItem : public FileDataCacheItem
{
public:

   double    fwhm;
   double    fwhmMeanDev;
   double    eccentricity;
   double    eccentricityMeanDev;
   double    psfSignalWeight;
   double    psfSNR;
   double    psfScale;
   double    psfScaleSNR;
   double    psfFlux;
   double    psfFluxPower;
   double    psfTotalMeanFlux;
   double    psfTotalMeanPowerFlux;
   unsigned  psfCount;
   double    MStar;
   double    NStar;
   double    snrWeight;
   double    median;
   double    medianMeanDev;
   double    noise;
   double    noiseRatio;
   unsigned  stars;
   double    starResidual;
   double    starResidualMeanDev;
   double    azimuth;
   double    altitude;

   SubframeSelectorCacheItem( const String& path = String(), const IsoString& key = IsoString() )
      : FileDataCacheItem( path, key )
   {
   }

private:

   void AssignData( const FileDataCacheItem& ) override;
   IsoStringList SerializedData() const override;
   bool DeserializeData( const IsoStringList& ) override;
};

// ----------------------------------------------------------------------------

class SubframeSelectorCache : public FileDataCache
{
public:

   SubframeSelectorCache();
   virtual ~SubframeSelectorCache();

   String CacheName() const override
   {
      return "SubframeSelector Cache";
   }

   int Version() const override
   {
      return 21;
   }

   int MinSupportedVersion() const override
   {
      return 21;
   }

   bool IsPersistent() const
   {
      return m_persistent;
   }

   void SetPersistent( bool );

   void SetMaxItemDuration( int days ) override;

private:

   bool m_persistent = true;

   FileDataCacheItem* NewItem() const override
   {
      return new SubframeSelectorCacheItem;
   }
};

extern SubframeSelectorCache* TheSubframeSelectorCache;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __SubframeSelectorCache_h

// ----------------------------------------------------------------------------
// EOF SubframeSelectorCache.h - Released 2024-12-11T17:43:17Z
