//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.02.01.0327
// ----------------------------------------------------------------------------
// IntegerResampleInstance.h - Released 2016/12/20 17:43:21 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __IntegerResampleInstance_h
#define __IntegerResampleInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/MetaParameter.h>
#include <pcl/Point.h>
#include <pcl/IntegerResample.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class IntegerResampleInstance : public ProcessImplementation
{
public:

   IntegerResampleInstance( const MetaProcess* );
   IntegerResampleInstance( const IntegerResampleInstance& );

   virtual void Assign( const ProcessImplementation& );

   virtual bool IsMaskable( const View&, const ImageWindow& ) const;

   virtual UndoFlags UndoMode( const View& ) const;

   virtual bool CanExecuteOn( const View&, pcl::String& whyNot ) const;
   virtual bool BeforeExecution( View& );
   virtual bool ExecuteOn( View& );

   virtual void* LockParameter( const MetaParameter*, size_type tableRow );

   void GetNewSizes( int& w, int& h ) const;

private:

   int32    p_zoomFactor;
   pcl_enum p_downsampleMode;
   DPoint   p_resolution;      // Horizontal and vertical resolution in pixels
   pcl_bool p_metric;          // Metric resolution?
   pcl_bool p_forceResolution; // Set resolution of target image window
   pcl_bool p_noGUIMessages;    // only show warning messages on the console

   friend class IntegerResampleEngine;
   friend class IntegerResampleInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __IntegerResampleInstance_h

// ----------------------------------------------------------------------------
// EOF IntegerResampleInstance.h - Released 2016/12/20 17:43:21 UTC
