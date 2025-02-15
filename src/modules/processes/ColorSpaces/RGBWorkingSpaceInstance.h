//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.1
// ----------------------------------------------------------------------------
// RGBWorkingSpaceInstance.h - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
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

#ifndef __RGBWorkingSpaceInstance_h
#define __RGBWorkingSpaceInstance_h

#include <pcl/ProcessImplementation.h>

namespace pcl
{

class RGBColorSystem;

// ----------------------------------------------------------------------------

class RGBWorkingSpaceInstance : public ProcessImplementation
{
public:

   RGBWorkingSpaceInstance( const MetaProcess* );
   RGBWorkingSpaceInstance( const RGBWorkingSpaceInstance& );
   RGBWorkingSpaceInstance( const MetaProcess*, const RGBColorSystem& );

   bool Validate( pcl::String& info ) override;
   void Assign( const ProcessImplementation& ) override;
   bool IsMaskable( const View& v, const ImageWindow& mask ) const override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View& v, String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   bool CanExecuteGlobal( String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

   const FVector& LuminanceCoefficients() const
   {
      return Y;
   }

   const FVector& ChromaticityXCoordinates() const
   {
      return x;
   }

   const FVector& ChromaticityYCoordinates() const
   {
      return y;
   }

   float Gamma() const
   {
      return gamma;
   }

   bool IsSRGB() const
   {
      return sRGB != 0;
   }

   bool AppliesGlobalRGBWS() const
   {
      return applyGlobalRGBWS != 0;
   }

   void NormalizeLuminanceCoefficients()
   {
      Y /= Y.Sum();
   }

private:

   FVector Y, x, y;
   float   gamma;
   uint32  sRGB;             // PCL MetaBoolean maps to uint32
   uint32  applyGlobalRGBWS; // ...

   friend class RGBWorkingSpaceInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __RGBWorkingSpaceInstance_h

// ----------------------------------------------------------------------------
// EOF RGBWorkingSpaceInstance.h - Released 2024-12-28T16:54:15Z
