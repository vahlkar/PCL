//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.4
// ----------------------------------------------------------------------------
// pcl/GaiaDR2.h - Released 2020-12-01T21:25:03Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2020 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_GaiaDR2_h
#define __PCL_GaiaDR2_h

/// \file pcl/GaiaDR2.h

#include <pcl/Defs.h>

#include <pcl/ElapsedTime.h>
#include <pcl/Flags.h>
#include <pcl/StarDatabaseFile.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::GaiaDR2StarFlag
 * \brief Data availability and quality flags for Gaia DR2 star data.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>GaiaDR2StarFlag::NoPM</td>           <td>No proper motions and parallax available.</td></tr>
 * <tr><td>GaiaDR2StarFlag::NoGBPMag</td>       <td>No G-BP magnitude available.</td></tr>
 * <tr><td>GaiaDR2StarFlag::NoGRPMag</td>       <td>No G-RP magnitude available.</td></tr>
 * <tr><td>GaiaDR2StarFlag::LackingData</td>    <td>Equivalent to NoPM|NoGBPMag|NoGRPMag.</td></tr>
 * <tr><td>GaiaDR2StarFlag::GoodRA</td>         <td>Standard error of right ascension < 1 mas.</td></tr>
 * <tr><td>GaiaDR2StarFlag::GoodDec</td>        <td>Standard error of declination < 1 mas.</td></tr>
 * <tr><td>GaiaDR2StarFlag::GoodParx</td>       <td>Standard error of parallax < 0.25 mas.</td></tr>
 * <tr><td>GaiaDR2StarFlag::GoodPMRA</td>       <td>Standard error of proper motion in right ascension < 0.75 mas/year.</td></tr>
 * <tr><td>GaiaDR2StarFlag::GoodPMDec</td>      <td>Standard error of proper motion in declination < 0.75 mas/year.</td></tr>
 * <tr><td>GaiaDR2StarFlag::GoodAstrometry</td> <td>Equivalent to GoodRA|GoodDec|GoodParx|GoodPMRA|GoodPMDec.</td></tr>
 * <tr><td>GaiaDR2StarFlag::BadRA</td>          <td>Standard error of right ascension > 2 mas.</td></tr>
 * <tr><td>GaiaDR2StarFlag::BadDec</td>         <td>Standard error of declination > 2 mas.</td></tr>
 * <tr><td>GaiaDR2StarFlag::BadParx</td>        <td>Standard error of parallax > 0.65 mas.</td></tr>
 * <tr><td>GaiaDR2StarFlag::BadPMRA</td>        <td>Standard error of proper motion in right ascension > 1.5 mas/year.</td></tr>
 * <tr><td>GaiaDR2StarFlag::BadPMDec</td>       <td>Standard error of proper motion in declination > 1.5 mas/year.</td></tr>
 * <tr><td>GaiaDR2StarFlag::BadAstrometry</td>  <td>Equivalent to BadRA|BadDec|BadParx|BadPMRA|BadPMDec.</td></tr>
 * <tr><td>GaiaDR2StarFlag::GoodGMag</td>       <td>Error on G-band mean flux < 1.5 e-/s.</td></tr>
 * <tr><td>GaiaDR2StarFlag::GoodGBPMag</td>     <td>Error on the integrated BP mean flux < 10 e-/s.</td></tr>
 * <tr><td>GaiaDR2StarFlag::GoodGRPMag</td>     <td>Error on the integrated RP mean flux < 10 e-/s.</td></tr>
 * <tr><td>GaiaDR2StarFlag::GoodPhotometry</td> <td>Equivalent to GoodGMag|GoodGBPMag|GoodGRPMag.</td></tr>
 * <tr><td>GaiaDR2StarFlag::BadGMag</td>        <td>Error on G-band mean flux > 2.5 e-/s.</td></tr>
 * <tr><td>GaiaDR2StarFlag::BadGBPMag</td>      <td>Error on the integrated BP mean flux > 20 e-/s, or BP/RP excess factor > 2.5.</td></tr>
 * <tr><td>GaiaDR2StarFlag::BadGRPMag</td>      <td>Error on the integrated RP mean flux > 20 e-/s, or BP/RP excess factor > 2.5.</td></tr>
 * <tr><td>GaiaDR2StarFlag::BadPhotometry</td>  <td>Equivalent to BadGMag|BadGBPMag|BadGRPMag.</td></tr>
 * </table>
 *
 * \ingroup point_source_databases
 */
namespace GaiaDR2StarFlag
{
   enum mask_type
   {
      NoPM           = 0x00000001,
      NoGBPMag       = 0x00000002,
      NoGRPMag       = 0x00000004,
      LackingData    = 0x00000007,

      GoodRA         = 0x00000010,
      GoodDec        = 0x00000020,
      GoodPMRA       = 0x00000040,
      GoodPMDec      = 0x00000080,

      BadRA          = 0x00000100,
      BadDec         = 0x00000200,
      BadPMRA        = 0x00000400,
      BadPMDec       = 0x00000800,

      GoodGMag       = 0x00001000,
      GoodGBPMag     = 0x00002000,
      GoodGRPMag     = 0x00004000,
      GoodParx       = 0x00008000,

      BadGMag        = 0x00010000,
      BadGBPMag      = 0x00020000,
      BadGRPMag      = 0x00040000,
      BadParx        = 0x00080000,

      GoodAstrometry = 0x000080F0,
      BadAstrometry  = 0x00080F00,
      GoodPhotometry = 0x00007000,
      BadPhotometry  = 0x00070000
   };
}

// ----------------------------------------------------------------------------

/*!
 * \struct GaiaDR2StarData
 * \brief Star data structure for Gaia DR2 search operations.
 *
 * \ingroup point_source_databases
 */
struct PCL_CLASS GaiaDR2StarData
{
   double ra = 0;     //!< Right ascension in degrees, in the range [0,360).
   double dec = 0;    //!< Declination in degrees, in the range [-90,+90].
   float  parx = 0;   //!< Parallax in mas.
   float  pmra = 0;   //!< Proper motion in right ascension * cos(dec), in mas/year.
   float  pmdec = 0;  //!< Proper motion in declination, in mas/year.
   float  magG = 0;   //!< Mean G magnitude.
   float  magBP = 0;  //!< Mean G_BP magnitude.
   float  magRP = 0;  //!< Mean G_RP magnitude.
   uint32 flags = 0u; //!< Data availability and quality flags. See the GaiaDR2StarFlag namespace.
};

// ----------------------------------------------------------------------------

/*!
 * \struct GaiaDR2SearchData
 * \brief Data items and parameters for Gaia DR2 search operations.
 *
 * \ingroup point_source_databases
 */
struct GaiaDR2SearchData
{
   double                 centerRA = 0;             //!< Field center right ascension coordinate in degrees (search parameter).
   double                 centerDec = 0;            //!< Field center declination coordinate in degrees (search parameter).
   double                 radius = 1;               //!< Field radius in degrees (search parameter).
   float                  magnitudeLow = -1.5;      /*!< Low magnitude (search parameter). Only stars of magnitude greater
                                                       than or equal to this value will be included in the stars list. */
   float                  magnitudeHigh = 26;       /*!< High magnitude (search parameter). Only stars of magnitude less
                                                       than or equal to this value will be included in the stars list. */
   uint32                 sourceLimit = uint32_max; /*!< The search will not include more objects than this limit
                                                       in the stars list (search parameter). */
   uint32                 inclusionFlags = 0u;      /*!< Inclusion flags (search parameter). If non-zero, only stars with
                                                       \e all of these flags set will be included in the stars list. */
   uint32                 exclusionFlags = 0u;      /*!< Exclusion flags (search parameter). Stars with any of these flags
                                                       set will \e not be included in the stars list. */

   Array<GaiaDR2StarData> stars;                    //!< The list of stars found by the search operation (output data).
   uint32                 excessCount = 0u;         /*!< When \a sourceLimit is exceeded, this is the number of
                                                       additional objects found but not included in the stars list (output data). */
   uint32                 rejectCount = 0u;         /*!< Total number of rejected objects (output data). This refers to
                                                       point sources that have been tested for inclusion in the search
                                                       result, but have not matched the search criteria. */
   double                 timeTotal = 0;            //!< Total search time in seconds (output data).
   double                 timeIO = 0;               //!< Time consumed by I/O operations in seconds (output data).
   uint32                 countIO = 0u;             //!< Total number of I/O operations performed (output data).
   double                 timeUncompress = 0;       //!< Time consumed by data uncompression in seconds (output data).
   double                 timeDecode = 0;           //!< Time consumed by data decoding in seconds (output data).

   /*!
    * Sets all search result data items to null values.
    */
   void ResetSearchResults()
   {
      stars.Clear();
      excessCount = rejectCount = 0u;
      timeTotal = timeIO = 0;
      countIO = 0u;
      timeUncompress = timeDecode = 0;
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class GaiaDR2DatabaseFile
 * \brief Gaia DR2 star database file (XPSD format).
 *
 * This class implements an interface to XPSD files serializing encoded Gaia
 * DR2 data. The most important functionality of this class is performing fast
 * indexed search operations to retrieve point source data for Gaia DR2 stars
 * matching a set of user-defined criteria. See the
 * GaiaDR2DatabaseFile::Search() member function and the GaiaDR2SearchData
 * structure for detailed information.
 *
 * This implementation provides the following data for the complete Gaia DR2
 * catalog:
 *
 * \li Source positions.
 * \li Parallaxes.
 * \li Proper motions.
 * \li Mean magnitudes on the G, GBP and GRP bands.
 * \li Data availability and quality flags.
 *
 * \b References
 *
 * \li Gaia Data Release 2 - online resources:
 * https://www.cosmos.esa.int/web/gaia/data-release-2
 *
 * \li <em>Gaia Data Release 2. Summary of the contents and survey
 * properties.</em> Gaia Collaboration, Brown, A.G.A., et al.:
 * https://arxiv.org/abs/1804.09365v2
 *
 * \li Gaia Data Release 2. Documentation release 1.2:
 * https://gea.esac.esa.int/archive/documentation/GDR2/index.html
 *
 * \sa StarDatabaseFile
 * \ingroup point_source_databases
 */
class PCL_CLASS GaiaDR2DatabaseFile : public StarDatabaseFile
{
public:

   /*!
    * Default constructor.
    *
    * Constructs an invalid instance that cannot be used until initialized by
    * calling the Open() member function.
    */
   GaiaDR2DatabaseFile() = default;

   /*!
    * Constructs a &GaiaDR2DatabaseFile instance initialized from the specified
    * point source database file in XPSD format.
    *
    * In the event of errors or invalid data, this constructor will throw the
    * appropriate Error exception.
    */
   GaiaDR2DatabaseFile( const String& filePath )
      : StarDatabaseFile( filePath )
   {
      static_assert( sizeof( EncodedStarData ) == 30, "Invalid sizeof( GaiaDR2DatabaseFile::EncodedStarData )" );
      if ( Metadata().databaseIdentifier != "GaiaDR2" )
         throw Error( "Not a valid GaiaDR2 database file: " + filePath );
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   GaiaDR2DatabaseFile& operator =( GaiaDR2DatabaseFile&& ) = default;

   /*!
    * Deleted copy constructor. %GaiaDR2DatabaseFile instances are unique,
    * hence cannot be copied.
    */
   GaiaDR2DatabaseFile( const GaiaDR2DatabaseFile& ) = delete;

   /*!
    * Deleted copy assignment operator. %GaiaDR2DatabaseFile instances are
    * unique, hence cannot be copied.
    */
   GaiaDR2DatabaseFile& operator =( const GaiaDR2DatabaseFile& ) = delete;

   /*!
    * Performs a search operation for point sources matching the specified
    * criteria.
    *
    * This member function performs a fast indexed search for point sources in
    * this database file matching the criteria defined in the specified \a data
    * structure. See the GaiaDR2SearchData structure for detailed information
    * on search parameters and output data.
    *
    * Summarily, search criteria include:
    *
    * \li The region of the sky where point sources will be searched for. This
    * region is defined by the equatorial coordinates of a field center and a
    * field radius.
    *
    * \li An optional range of magnitudes.
    *
    * \li Optional inclusion/exclusion flags.
    *
    * \li An optional limit for the number of sources included in the search
    * result.
    *
    * The result of the search operation is also returned in the specified
    * \a data structure, including, among others, the following items:
    *
    * \li The list of point sources found.
    *
    * \li Instrumentation items for performance analysis, including: total
    * search time, time used for I/O operations, total I/O operations, time
    * used for data decoding, and time used for data decompression.
    */
   void Search( GaiaDR2SearchData& data ) const
   {
      ElapsedTime T;
      for ( const XPSD::IndexTree& tree : m_index )
         tree.Search( data.centerRA, data.centerDec, data.radius, &data );
      data.timeTotal += T();
   }

private:

#pragma pack(push, 1)
   /*
    * Encoded star record (30 bytes uncompressed).
    */
   struct EncodedStarData
   {
      // Projected coordinates relative to parent node origin, in 0.002 mas units.
      uint32 dx;
      uint32 dy;
      // Parallax, in mas.
      float  parx;
      // Proper motions, mas/yr.
      float  pmra;
      float  pmdec;
      // Mean magnitudes in 0.001 mag units, encoded as (mag + 1.5)*1000.
      uint16 magG;
      uint16 magBP;
      uint16 magRP;
#ifdef _MSC_VER
      uint32 flagsAndDRA;
#else
      // Data availability and quality flags.
      uint32 flags : 20;
      // Right ascension correction for high declinations, in 0.01 mas units.
      int    dra   : 12;
#endif

      uint32 Flags() const
      {
#ifdef _MSC_VER
         return flagsAndDRA & 0x000FFFFF;
#else
         return flags;
#endif
      }

      int DRA() const
      {
#ifdef _MSC_VER
         return int( (flagsAndDRA & 0xFFF00000) >> 20 );
#else
         return dra;
#endif
      }
   };
#pragma pack(pop)

   void LoadData( void* block, uint64 offset, uint32 size, void* searchData ) const override
   {
      ElapsedTime T;
      StarDatabaseFile::LoadData( block, offset, size, searchData );
      reinterpret_cast<GaiaDR2SearchData*>( searchData )->timeIO += T();
      ++reinterpret_cast<GaiaDR2SearchData*>( searchData )->countIO;
   }

   void Uncompress( ByteArray& block, uint32 uncompressedSize, void* searchData ) const override
   {
      ElapsedTime T;
      StarDatabaseFile::Uncompress( block, uncompressedSize, searchData );
      reinterpret_cast<GaiaDR2SearchData*>( searchData )->timeUncompress += T();
   }

   void GetEncodedData( const ByteArray& data, const XPSD::IndexTree& tree, const XPSD::IndexNode& node, void* searchData ) const override
   {
      ElapsedTime T;
      GaiaDR2SearchData* search = reinterpret_cast<GaiaDR2SearchData*>( searchData );
      double r = Rad( search->radius );
      const EncodedStarData* S = reinterpret_cast<const EncodedStarData*>( data.Begin() );
      int count = int( data.Size() / sizeof( EncodedStarData ) );
      int matched = 0;
      for ( int i = 0; i < count; ++i, ++S )
         if ( search->inclusionFlags == 0 || (S->Flags() & search->inclusionFlags) == search->inclusionFlags )
            if ( (S->Flags() & search->exclusionFlags) == 0 )
            {
               float magG = 0.001*S->magG - 1.5;
               if ( magG >= search->magnitudeLow )
                  if ( magG <= search->magnitudeHigh )
                  {
                     GaiaDR2StarData star;
                     double x = node.x0 + double( S->dx )/3600/1000/500;
                     double y = node.y0 + double( S->dy )/3600/1000/500;
                     tree.Unproject( star.ra, star.dec, x, y );
                     if ( unlikely( S->DRA() != 0 ) )
                     {
                        star.ra += double( S->DRA() )/3600/1000/100;
                        if ( star.ra < 0 )
                           star.ra += 360;
                        else if ( star.ra >= 360 )
                           star.ra -= 360;
                     }
                     if ( Distance( search->centerRA, search->centerDec, star.ra, star.dec ) < r )
                     {
                        if ( search->stars.Length() < size_type( search->sourceLimit ) )
                        {
                           star.parx = S->parx;
                           star.pmra = S->pmra;
                           star.pmdec = S->pmdec;
                           star.magG = magG;
                           star.magBP = 0.001*S->magBP - 1.5;
                           star.magRP = 0.001*S->magRP - 1.5;
                           star.flags = S->Flags();
                           search->stars << star;
                        }
                        else
                           ++search->excessCount;
                        ++matched;
                     }
                  }
            }

      search->rejectCount += count - matched;
      search->timeDecode += T();
   }

   friend class GaiaDR2DatabaseFileGenerator;
};

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_GaiaDR2_h

// ----------------------------------------------------------------------------
// EOF pcl/GaiaDR2.h - Released 2020-12-01T21:25:03Z
