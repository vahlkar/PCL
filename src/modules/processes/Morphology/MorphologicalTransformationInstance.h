//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard Morphology Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// MorphologicalTransformationInstance.h - Released 2023-08-28T15:23:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard Morphology PixInsight module.
//
// Copyright (c) 2003-2023 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __MorphologicalTransformationInstance_h
#define __MorphologicalTransformationInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/Image.h>

#include "MorphologicalTransformationParameters.h"
#include "Structure.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class MorphologicalTransformationInstance : public ProcessImplementation
{
public:

   MorphologicalTransformationInstance( const MetaProcess* );
   MorphologicalTransformationInstance( const MorphologicalTransformationInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View& v, String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

   int Operator() const
   {
      return morphologicalOperator;
   }

   int InterlacingDistance() const
   {
      return interlacingDistance;
   }

   double LowThreshold() const
   {
      return lowThreshold;
   }

   double HighThreshold() const
   {
      return highThreshold;
   }

   int NumberOfIterations() const
   {
      return numberOfIterations;
   }

   float Amount() const
   {
      return amount;
   }

   float SelectionPoint() const
   {
      return selectionPoint;
   }

   const Structure& GetStructure() const
   {
      return structure;
   }

   Structure& GetStructure()
   {
      return structure;
   }

private:

   pcl_enum  morphologicalOperator;
   uint32    interlacingDistance;
   double    lowThreshold;
   double    highThreshold;
   uint32    numberOfIterations;
   float     amount;
   float     selectionPoint; // for the selection filter only
   Structure structure;

   friend class MorphologicalTransformationInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __MorphologicalTransformationInstance_h

// ----------------------------------------------------------------------------
// EOF MorphologicalTransformationInstance.h - Released 2023-08-28T15:23:41Z
