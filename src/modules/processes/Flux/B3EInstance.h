//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard Flux Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// B3EInstance.h - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard Flux PixInsight module.
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

#ifndef __B3EInstance_h
#define __B3EInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/MetaParameter.h> // pcl_bool, pcl_enum

namespace pcl
{

// ----------------------------------------------------------------------------

class B3EInstance : public ProcessImplementation
{
public:

   B3EInstance( const MetaProcess* );
   B3EInstance( const B3EInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool Validate( String& info ) override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool CanExecuteGlobal( pcl::String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

private:

   // Process parameters

   struct InputViewParameters
   {
      InputViewParameters();
      InputViewParameters( const InputViewParameters& ) = default;

      InputViewParameters& operator =( const InputViewParameters& ) = default;

      String      id;
      double      center = 0;
      pcl_bool    subtractBackground;
      String      backgroundReferenceViewId;
      float       backgroundLow;
      float       backgroundHigh;
      pcl_bool    backgroundUseROI;
      Rect        backgroundROI = 0;
      pcl_bool    outputBackgroundReferenceMask;
   };

   InputViewParameters  p_inputView[ 2 ];

   /*
   String      p_inputViewId1;
   String      p_inputViewId2;
   double      p_inputCenter1;
   double      p_inputCenter2;
   */
   double      p_outputCenter = 0;
   pcl_enum    p_intensityUnits;
   pcl_bool    p_syntheticImage;
   pcl_bool    p_thermalMap;
   pcl_bool    p_outOfRangeMask;

   /*
   struct BackgroundParameters
   {
      // Background calibration 1
      pcl_bool    subtractBackground;
      String      backgroundReferenceViewId;
      float       backgroundLow;
      float       backgroundHigh;
      pcl_bool    backgroundUseROI;
      Rect        backgroundROI;
      pcl_bool    outputBackgroundReferenceMask;
   } p_background[2];
   */

   // Read-only properties

   String      o_syntheticImageViewId;
   String      o_thermalMapViewId;
   String      o_outOfRangeMaskViewId;

   static double EvaluateBackground( const InputViewParameters& imgParams );

   friend class B3EEngine;
   friend class B3EProcess;
   friend class B3EInterface;
};

// ----------------------------------------------------------------------------


} // pcl

#endif   // __B3EInstance_h

// ----------------------------------------------------------------------------
// EOF B3EInstance.h - Released 2021-04-09T19:41:48Z
