//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard CloneStamp Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// CloneStampInstance.h - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard CloneStamp PixInsight module.
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

#ifndef __CloneStampInstance_h
#define __CloneStampInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/Rectangle.h>
#include <pcl/KernelFilter.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class CloneStampInstance : public ProcessImplementation
{
public:

   CloneStampInstance( const MetaProcess* );
   CloneStampInstance( const CloneStampInstance& );

   void Assign( const ProcessImplementation& ) override;
   void TestAssign( const ProcessImplementation& );
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View& v, String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   bool IsMaskable( const View& v, const ImageWindow& mask ) const override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

private:

   struct BrushData
   {
      /*
       * Brush radius. Radius=0 means a single-point brush.
       */
      int32 radius = 5;

      /*
       * Brush softness. From 0=hard to 1=very_soft. 0.5=Gaussian.
       */
      float softness = 0.5F;

      /*
       * Brush opacity. 0=transparent (no action), 1=opaque
       * Opacity multiplies mask items before mixing cloned pixels.
       */
      float opacity = 1.0F;

      BrushData() = default;
      BrushData( const BrushData& ) = default;

      bool operator ==( const BrushData& b ) const
      {
         return radius == b.radius && softness == b.softness && opacity == b.opacity;
      }

      KernelFilter CreateBrush() const;
   };

   struct ClonerData
   {
      uint32   actionIdx = uint32_max; // action index (on the actions array)
      I32Point targetPos; // target location

      ClonerData( const Point& p = Point( 0 ) ) : targetPos( p )
      {
      }

      ClonerData( const ClonerData& ) = default;
   };

   struct ActionData
   {
      String    sourceId;         // source image
      int32     sourceWidth = 0;  // source width in pixels
      int32     sourceHeight = 0; // source height in pixels
      I32Point  sourcePos = 0;    // initial source position
      BrushData brush;            // action brush

      ActionData() = default;
      ActionData( const ActionData& ) = default;
   };

   typedef Array<ClonerData>  cloner_sequence;
   typedef Array<ActionData>  action_sequence;

   action_sequence actions; // cloner actions
   cloner_sequence cloner;  // cloner items

   int32 width = 0;  // target width in pixels
   int32 height = 0; // target height in pixels

   static void Apply( ImageVariant& img, const ImageVariant& src, const ImageVariant& mask, bool maskInverted,
                      const BrushData&, cloner_sequence::const_iterator, const Point& delta );

   /*
    * Interface data
    */
   uint32 color;       // cloner color
   uint32 boundsColor; // bounds color

   // Private flag to work in coordination with CloneStampInterface::Execute()
   bool   isInterfaceInstance = false;

   friend class CloneStampEngine;
   friend class CloneStampInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __CloneStampInstance_h

// ----------------------------------------------------------------------------
// EOF CloneStampInstance.h - Released 2024-12-28T16:54:15Z
