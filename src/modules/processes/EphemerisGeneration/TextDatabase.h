//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard EphemerisGeneration Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// TextDatabase.h - Released 2021-04-09T19:41:48Z
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

#ifndef __TextBatabase_h
#define __TextBatabase_h

#include "EphemerisGeneratorInstance.h"

#include <pcl/Optional.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Parsing and decoding plain text databases of orbital elements and state
 * vectors.
 *
 * This mostly includes well-known public databases from Lowell Observatory,
 * Minor Planet Center and Jet Propulsion Laboratory in a variety of formats,
 * plus some generic CSV files and tabular formats generated by our standard
 * scripts and tools.
 */
class TextDatabase
{
public:

   /*
    * Enumerated types describing database types, field types and formats.
    */
   enum contents_type
   {
      Contents_OrbitalElements,
      Contents_StateVectors
   };
   enum field_disposition_type
   {
      FieldDisposition_CommaSeparated,
      FieldDisposition_FixedWidth
   };
   enum date_format_type
   {
      DateFormat_ISO,
      DateFormat_Compact,
      DateFormat_Separate,
      DateFormat_JD,
      DateFormat_MJD,
      DateFormat_PackedMPC
   };
   enum id_format_type
   {
      IdFormat_Plain,
      IdFormat_PackedMPC,
      IdFormat_Parenthesized
   };

   /*
    * Description of a database field.
    */
   struct Field
   {
      int pos   = 0;
      int width = -1;

      operator bool() const
      {
         return width >= 0;
      }
   };

   /*
    * Description of a database format
    */
   struct FormatDescription
   {
      String                 name;
      contents_type          contents = Contents_OrbitalElements;
      field_disposition_type fieldDisposition = FieldDisposition_CommaSeparated;
      date_format_type       epochFormat = DateFormat_Compact;
      date_format_type       timePPFormat = DateFormat_JD;
      id_format_type         idFormat = IdFormat_Plain;
      int                    skipHeaderLines = 0;
      Field fld_id;
      Field fld_name;
      Field fld_epoch;
      Field fld_H;
      Field fld_G;
      Field fld_B_V;
      Field fld_D;
      Field fld_a;
      Field fld_q;
      Field fld_e;
      Field fld_M;
      Field fld_T;
      Field fld_i;
      Field fld_O;
      Field fld_w;
      Field fld_x;
      Field fld_y;
      Field fld_z;
      Field fld_vx;
      Field fld_vy;
      Field fld_vz;

      bool operator ==( const FormatDescription& f ) const
      {
         return name.CompareIC( f.name ) == 0;
      }

      bool operator <( const FormatDescription& f ) const
      {
         return name.CompareIC( f.name ) < 0;
      }

      void Validate() const;

      static Array<FormatDescription> Parse( const String& filePath );
   };

   /*
    * Data retrieved for an object during a database search operation.
    */
   struct ObjectData
   {
      IsoString        id;      // unique object identifier
      IsoString        name;    // object name
      Vector           state = Vector( 0.0, 8 ); // state vectors or orbital elements
      double           epochJD; // epoch of initial conditions, JD, TDB
      Optional<double> H;       // absolute magnitude (mag)
      Optional<double> G;       // slope parameter
      Optional<double> B_V;     // color index (mag)
      Optional<double> D;       // diameter (km)

      bool operator ==( const ObjectData& x ) const
      {
         return id == x.id && name == x.name;
      }

      bool operator <( const ObjectData& x ) const
      {
         if ( id.IsEmpty() )
         {
            if ( !x.id.IsEmpty() )
               return false;
            return name < x.name;
         }
         else
         {
            if ( x.id.IsEmpty() )
               return true;

            int id1, id2;
            if ( id.TryToInt( id1, 10/*base*/ ) )
               if ( x.id.TryToInt( id2, 10/*base*/ ) )
                  if ( id1 != id2 )
                     return id1 < id2;
            return (id != x.id) ? id < x.id : name < x.name;
         }
      }
   };

   TextDatabase( const FormatDescription& format )
      : m_format( format )
   {
   }

   virtual ~TextDatabase() = default;

   Array<ObjectData> Search( const EphemerisGeneratorInstance&, size_type maxCount = 0 ) const;

private:

   FormatDescription m_format;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __TextBatabase_h

// ----------------------------------------------------------------------------
// EOF TextDatabase.h - Released 2021-04-09T19:41:48Z