//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.0
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 1.25.0
// ----------------------------------------------------------------------------
// IntegrationCache.cpp - Released 2020-07-31T19:33:39Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
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
//    by Pleiades Astrophoto and its contributors (http://pixinsight.com/)."
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

static bool GetTwoSidedEstimates( IntegrationFile::scale_estimates& v,
                                  StringList::const_iterator& i, const StringList& s )
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
      StringList tokens;
      i->Break( tokens, ',' );
      if ( tokens.Length() != 2 )
         throw ParseError( "Parsing two-sided scale estimate: wrong number of components" );
      v[j].low = tokens[0].ToDouble();
      v[j].high = tokens[1].ToDouble();
   }
   return true;
}

static String TwoSidedEstimatesAsString( const IntegrationFile::scale_estimates& v )
{
   String s = String().Format( "\n%d", v.Length() );
   for ( int i = 0; i < v.Length(); ++i )
      s.AppendFormat( "\n%.8e,%.8e", v[i].low, v[i].high );
   return s;
}

// ----------------------------------------------------------------------------

void IntegrationCacheItem::AssignData( const FileDataCacheItem& item )
{
#define src static_cast<const IntegrationCacheItem&>( item )
   mean     = src.mean;
   median   = src.median;
   avgDev   = src.avgDev;
   mad      = src.mad;
   bwmv     = src.bwmv;
   noise    = src.noise;
   ax       = src.ax;
   ay       = src.ay;
   am       = src.am;
   as0      = src.as0;
   as1      = src.as1;
   metadata = src.metadata;
#undef src
}

// ----------------------------------------------------------------------------

String IntegrationCacheItem::DataAsString() const
{
   StringList tokens;
   if ( !mean.IsEmpty() )
      tokens.Append( "mean" + VectorAsString( mean ) );
   if ( !median.IsEmpty() )
      tokens.Append( "median" + VectorAsString( median ) );
   if ( !avgDev.IsEmpty() )
      tokens.Append( "avgDev" + TwoSidedEstimatesAsString( avgDev ) );
   if ( !mad.IsEmpty() )
      tokens.Append( "mad" + TwoSidedEstimatesAsString( mad ) );
   if ( !bwmv.IsEmpty() )
      tokens.Append( "bwmv" + TwoSidedEstimatesAsString( bwmv ) );
   if ( !noise.IsEmpty() )
      tokens.Append( "noise" + VectorAsString( noise ) );
   if ( !ax.IsEmpty() )
      tokens.Append( "ax" + VectorAsString( ax ) );
   if ( !ay.IsEmpty() )
      tokens.Append( "ay" + VectorAsString( ay ) );
   if ( !am.IsEmpty() )
      tokens.Append( "am" + MultiVectorAsString( am ) );
   if ( !as0.IsEmpty() )
      tokens.Append( "as0" + MultiVectorAsString( as0 ) );
   if ( !as1.IsEmpty() )
      tokens.Append( "as1" + MultiVectorAsString( as1 ) );
   if ( !metadata.IsEmpty() )
      tokens.Append( "metadata\n" + metadata );

   return String().ToNewLineSeparated( tokens );
}

// ----------------------------------------------------------------------------

bool IntegrationCacheItem::GetDataFromTokens( const StringList& tokens )
{
   AssignData( IntegrationCacheItem() );

   for ( StringList::const_iterator i = tokens.Begin(); i != tokens.End(); )
      if ( *i == "mean" )
      {
         if ( !GetVector( mean, ++i, tokens ) )
            return false;
      }
      else if ( *i == "median" )
      {
         if ( !GetVector( median, ++i, tokens ) )
            return false;
      }
      else if ( *i == "avgDev" )
      {
         if ( !GetTwoSidedEstimates( avgDev, ++i, tokens ) )
            return false;
      }
      else if ( *i == "mad" )
      {
         if ( !GetTwoSidedEstimates( mad, ++i, tokens ) )
            return false;
      }
      else if ( *i == "bwmv" )
      {
         if ( !GetTwoSidedEstimates( bwmv, ++i, tokens ) )
            return false;
      }
      else if ( *i == "noise" )
      {
         if ( !GetVector( noise, ++i, tokens ) )
            return false;
      }
      else if ( *i == "ax" )
      {
         if ( !GetVector( ax, ++i, tokens ) )
            return false;
      }
      else if ( *i == "ay" )
      {
         if ( !GetVector( ay, ++i, tokens ) )
            return false;
      }
      else if ( *i == "am" )
      {
         if ( !GetMultiVector( am, ++i, tokens ) )
            return false;
      }
      else if ( *i == "as0" )
      {
         if ( !GetMultiVector( as0, ++i, tokens ) )
            return false;
      }
      else if ( *i == "as1" )
      {
         if ( !GetMultiVector( as1, ++i, tokens ) )
            return false;
      }
      else if ( *i == "metadata" )
      {
         if ( ++i == tokens.End() )
            return false;
         metadata = *i;
      }
      else
      {
         ++i;
      }

   return true;
}

// ----------------------------------------------------------------------------

IntegrationCache::IntegrationCache()
   : FileDataCache( "/ImageIntegration/Cache" )
{
   if ( TheIntegrationCache == nullptr )
      TheIntegrationCache = this;
   Load();
}

// ----------------------------------------------------------------------------

IntegrationCache::~IntegrationCache()
{
   if ( TheIntegrationCache == this )
      TheIntegrationCache = nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF IntegrationCache.cpp - Released 2020-07-31T19:33:39Z
