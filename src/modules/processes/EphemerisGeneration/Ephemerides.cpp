//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard EphemerisGeneration Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// Ephemerides.cpp - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard EphemerisGeneration PixInsight module.
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

#include "Ephemerides.h"

#include <pcl/GlobalSettings.h>
#include <pcl/SortedArray.h>

namespace pcl
{

// ----------------------------------------------------------------------------

Ephemerides::Ephemerides( const EphemerisGeneratorInstance& instance, bool fundamentalOnly )
{
   if ( instance.p_fundamentalFilePath.IsEmpty() )
   {
      String filePath = PixInsightSettings::GlobalString( "Application/FundamentalEphemeridesFilePath" );
      if ( filePath.IsEmpty() )
         throw Error( "The core fundamental ephemerides file has not been defined." );
      m_eph = new EphemerisFile( filePath );
   }
   else
      m_eph = new EphemerisFile( instance.p_fundamentalFilePath );

   if ( !fundamentalOnly )
   {
      if ( instance.p_useAsteroidPerturbers )
         if ( instance.p_asteroidsFilePath.IsEmpty() )
         {
            String filePath = PixInsightSettings::GlobalString( "Application/AsteroidEphemeridesFilePath" );
            if ( filePath.IsEmpty() )
               throw Error( "The core asteroid ephemerides file has not been defined." );
            m_ast = new EphemerisFile( filePath );
         }
         else
            m_ast = new EphemerisFile( instance.p_asteroidsFilePath );

      if ( instance.p_useKBOPerturbers )
      {
         if ( instance.p_KBOsFilePath.IsEmpty() )
         {
            String filePath = PixInsightSettings::GlobalString( "Application/KBOEphemeridesFilePath" );
            if ( filePath.IsEmpty() )
               throw Error( "The core KBO ephemerides file has not been defined." );
            m_kbo = new EphemerisFile( filePath );
         }
         else
            m_kbo = new EphemerisFile( instance.p_KBOsFilePath );
      }
   }
}

// ----------------------------------------------------------------------------

Ephemerides::~Ephemerides()
{
   delete m_eph;
   delete m_ast;
   delete m_kbo;
}

// ----------------------------------------------------------------------------

double Ephemerides::GMS() const
{
   return m_eph->ConstantValue( "GMS" );
}

// ----------------------------------------------------------------------------

struct KBOData
{
   IsoString id;
   String    name;
   double    M;

   bool operator ==( const KBOData& x ) const
   {
      return id.IsEmpty() ? name.CompareIC( x.name ) == 0 : id == x.id;
   }

   bool operator <( const KBOData& x ) const
   {
      if ( id.IsEmpty() )
         return !x.id.IsEmpty() || name.CompareIC( x.name ) < 0;
      return !x.id.IsEmpty() && id < x.id;
   }
};

static SortedArray<KBOData> s_knownKBOs;

double Ephemerides::KBOMass( const EphemerisObject& obj )
{
   static Mutex mutex;
   {
      volatile AutoLock lock( mutex );

      /*
       * The following mass parameters have been taken from DE440 headers
       * (MA8001 ... MA8030 integration constants).
       */
      if ( s_knownKBOs.IsEmpty() )
         s_knownKBOs << KBOData{ "136199", "Eris",       2.48544803028732e-12 }
                     << KBOData{ "136108", "Haumea",     5.96163005450428e-13 }
                     << KBOData{ "90377",  "Sedna",      1.49348678842449e-13 }
                     << KBOData{ "136472", "Makemake",   3.43185550062921e-13 }
                     << KBOData{ "50000",  "Quaoar",     1.52948774597511e-13 }
                     << KBOData{ "84522",  "2002 TC302", 2.28199634312045e-13 }
                     << KBOData{ "90482",  "Orcus",      9.40962464315611e-14 }
                     << KBOData{ "20000",  "Varuna",     5.49507522341186e-14 }
                     << KBOData{ "55637",  "2002 UX25",  1.86035124454487e-14 }
                     << KBOData{ "28978",  "Ixion",      4.51766562250976e-14 }
                     << KBOData{ "307261", "2002 MS4",   7.65350174906720e-14 }
                     << KBOData{ "174567", "Varda",      3.96577688696419e-14 }
                     << KBOData{ "",       "2006 QH181", 6.99160258973788e-14 }
                     << KBOData{ "",       "2004 XR190", 2.90409951818848e-14 }
                     << KBOData{ "55565",  "2002 AW197", 6.02473242459976e-14 }
                     << KBOData{ "145452", "2005 RN43",  3.62854799177896e-14 }
                     << KBOData{ "90568",  "2004 GV9",   3.71935023738244e-14 }
                     << KBOData{ "208996", "2003 AZ84",  6.08212352839477e-14 }
                     << KBOData{ "225088", "Gonggong",   2.58567882770904e-13 }
                     << KBOData{ "19521",  "Chaos",      2.53837964562973e-14 }
                     << KBOData{ "120347", "Salacia",    6.51170207936313e-14 }
                     << KBOData{ "278361", "2007 JJ43",  2.67691589753377e-14 }
                     << KBOData{ "",       "2010 KZ39",  2.95798751569601e-14 }
                     << KBOData{ "230965", "2004 XA192", 3.64502260200911e-15 }
                     << KBOData{ "42301",  "2001 UR163", 2.72519389203377e-14 }
                     << KBOData{ "455502", "2003 UZ413", 6.48385807558172e-14 }
                     << KBOData{ "",       "2010 RF43",  1.75614259349073e-14 }
                     << KBOData{ "523639", "2010 RE64",  1.91288669686944e-14 }
                     << KBOData{ "528381", "2008 ST291", 2.13389035100458e-14 }
                     << KBOData{ "",       "2010 FX86",  2.57103425708102e-14 };
   }

   SortedArray<KBOData>::const_iterator i = s_knownKBOs.Search( KBOData{ obj.objectId, obj.objectName, 0 } );
   return (i == s_knownKBOs.End()) ? 0.0 : i->M;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF Ephemerides.cpp - Released 2021-04-09T19:41:48Z
