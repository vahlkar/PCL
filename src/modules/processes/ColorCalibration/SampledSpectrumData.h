//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.5
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// SampledSpectrumData.h - Released 2024-01-13T15:48:22Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
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

#ifndef __SampledSpectrumData_h
#define __SampledSpectrumData_h

#include <pcl/Array.h>
#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

struct SampledSpectrumData
{
   String name;
   String channel;     // R,G,B or empty=unassigned
   String defaultItem; // channel this should be used as default (W,R,G,B,Q)
   String reference;   // published source of the data
   String data;        // CSV: <wavelength_1>,<value_1>, ...,<wavelength_n>,<value_n>

   bool IsValid() const
   {
      return !name.IsEmpty() && !data.IsEmpty();
   }

   bool operator ==( const SampledSpectrumData& other ) const
   {
      return name.CompareIC( other.name ) == 0;
   }

   bool operator <( const SampledSpectrumData& other ) const
   {
      return name.CompareIC( other.name ) < 0;
   }
};

class SampledSpectrumDataList
{
public:

   using list_type = Array<SampledSpectrumData>;
   using iterator = list_type::iterator;
   using const_iterator = list_type::const_iterator;

   SampledSpectrumDataList() = default;
   SampledSpectrumDataList( const SampledSpectrumDataList& ) = default;

   int Length() const
   {
      return int( m_list.Length() );
   }

   bool IsEmpty() const
   {
      return m_list.IsEmpty();
   }

   int Find( const String& name ) const
   {
      if ( m_sorted )
      {
         const_iterator i = BinarySearch( Begin(), End(), SampledSpectrumData{ name, String(), String(), String() } );
         if ( i != End() )
            return int( i - Begin() );
      }
      else
      {
         for ( int i = 0, n = Length(); i < n; ++i )
            if ( m_list[i].name == name )
               return i;
      }
      return -1;
   }

   int Find( const SampledSpectrumData& data ) const
   {
      return Find( data.name );
   }

   bool Has( const String& name ) const
   {
      return Find( name ) >= 0;
   }

   int Has( const SampledSpectrumData& data ) const
   {
      return Find( data ) >= 0;
   }

   iterator Begin()
   {
      return m_list.Begin();
   }

   const_iterator Begin() const
   {
      return m_list.Begin();
   }

   iterator begin()
   {
      return Begin();
   }

   const_iterator begin() const
   {
      return Begin();
   }

   iterator End()
   {
      return m_list.End();
   }

   const_iterator End() const
   {
      return m_list.End();
   }

   iterator end()
   {
      return End();
   }

   const_iterator end() const
   {
      return End();
   }

   const SampledSpectrumData& operator[]( int i ) const
   {
      return m_list[i];
   }

   SampledSpectrumData& operator[]( int i )
   {
      return m_list[i];
   }

   const SampledSpectrumData& First() const
   {
      PCL_PRECONDITION( !IsEmpty() )
      return *m_list.Begin();
   }

   const SampledSpectrumData& Last() const
   {
      PCL_PRECONDITION( !IsEmpty() )
      return *m_list.ReverseBegin();
   }

   void Set( const SampledSpectrumData& s )
   {
      if ( m_list.IsEmpty() )
      {
         m_list << s;
         m_sorted = true;
      }
      else
      {
         int i = Find( s.name );
         if ( i < 0 )
         {
            m_sorted = m_sorted && s > Last();
            m_list << s;
         }
         else
            m_list[i] = s;
      }
   }

   SampledSpectrumDataList& operator <<( const SampledSpectrumData& s )
   {
      Set( s );
      return *this;
   }

   bool IsSorted() const
   {
      return m_sorted;
   }

   void Sort()
   {
      if ( !m_sorted )
      {
         m_list.Sort();
         m_sorted = true;
      }
   }

   void Clear()
   {
      m_list.Clear();
      m_sorted = false;
   }

   String UniqueItemName( const String& name )
   {
      String uniqueName = name;
      for ( unsigned i = 0;; )
      {
         if ( !Find( uniqueName ) )
            break;
         uniqueName = name + String().Format( " (%u)", ++i );
      }
      return uniqueName;
   }

private:

   list_type m_list;
   bool      m_sorted = false;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __SampledSpectrumData_h

// ----------------------------------------------------------------------------
// EOF SampledSpectrumData.h - Released 2024-01-13T15:48:22Z
