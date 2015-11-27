//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0342
// ----------------------------------------------------------------------------
// FITSHeaderInstance.h - Released 2015/11/26 16:00:12 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __FITSHeaderInstance_h
#define __FITSHeaderInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/FITSHeaderKeyword.h>

#include "FITSHeaderParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------
// FITSHeaderInstance
// ----------------------------------------------------------------------------

class FITSHeaderInstance : public ProcessImplementation
{
public:

   struct Keyword
   {
      String name, value, comment;

      Keyword() :
      name(), value(), comment()
      {
      }

      Keyword( const Keyword& k ) :
      name( k.name ), value( k.value ), comment( k.comment )
      {
      }

      template <class S>
      Keyword( const S& n, const S& v, const S& c ) :
      name( String( n ) ), value( String( v ) ), comment( String( c ) )
      {
      }
   };

   typedef Array<Keyword>  keyword_list;

   FITSHeaderInstance( const MetaProcess* );
   FITSHeaderInstance( const FITSHeaderInstance& );

   virtual void Assign( const ProcessImplementation& );
   virtual UndoFlags UndoMode( const View& ) const;
   virtual bool IsMaskable( const View&, const ImageWindow& ) const;
   virtual bool CanExecuteOn( const View&, String& whyNot ) const;
   virtual bool ExecuteOn( View& );

   virtual void* LockParameter( const MetaParameter*, size_type /*tableRow*/ );
   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter*, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

   // -------------------------------------------------------------------------

   void ImportKeywords( const ImageWindow& );

   static bool IsReservedKeyword( const String& id );

   // -------------------------------------------------------------------------

private:

   keyword_list keywords;

   friend class FITSHeaderProcess;
   friend class FITSHeaderInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FITSHeaderInstance_h

// ----------------------------------------------------------------------------
// EOF FITSHeaderInstance.h - Released 2015/11/26 16:00:12 UTC
