//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard EphemerisGeneration Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// Ephemerides.h - Released 2021-04-09T19:41:48Z
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

#ifndef __Ephemerides_h
#define __Ephemerides_h

#include "EphemerisGeneratorInstance.h"

#include <pcl/EphemerisFile.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Fundamental and massive asteroid ephemerides.
 */
class Ephemerides
{
public:

   Ephemerides( const EphemerisGeneratorInstance&, bool fundamentalOnly = false );

   virtual ~Ephemerides();

   /*
    * Fundamental ephemerides.
    */
   EphemerisFile& Fundamental() const
   {
      return *m_eph;
   }

   /*
    * Massive asteroid ephemerides.
    */
   EphemerisFile& Asteroids() const
   {
      return *m_ast;
   }

   /*
    * Massive Kuiper Belt object (KBO) ephemerides.
    */
   EphemerisFile& KBOs() const
   {
      return *m_kbo;
   }

   /*
    * The solar gravitational mass parameter, acquired directly from
    * fundamental ephemerides.
    */
   double GMS() const;

   /*
    * KBO mass parameters from DE440.
    */
   static double KBOMass( const EphemerisObject& );

private:

   EphemerisFile* m_eph = nullptr; // fundamental ephemerides
   EphemerisFile* m_ast = nullptr; // asteroid ephemerides
   EphemerisFile* m_kbo = nullptr; // KBO ephemerides
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __Ephemerides_h

// ----------------------------------------------------------------------------
// EOF Ephemerides.h - Released 2021-04-09T19:41:48Z
