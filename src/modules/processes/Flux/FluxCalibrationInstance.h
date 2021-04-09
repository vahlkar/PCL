//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard Flux Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// FluxCalibrationInstance.h - Released 2021-04-09T19:41:48Z
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

#ifndef __FluxCalibrationInstance_h
#define __FluxCalibrationInstance_h

#include "FluxCalibrationParameters.h"

#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class FluxCalibrationInstance : public ProcessImplementation
{
public:

   FluxCalibrationInstance( const MetaProcess* );
   FluxCalibrationInstance( const FluxCalibrationInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter*, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter*, size_type tableRow ) const override;

private:

   struct CalibrationParameter
   {
      float    value;
      pcl_enum mode;
      String   keyword;
      String   stdKeyword;

      CalibrationParameter( float a_value, pcl_enum a_mode, const String& a_stdKeyword )
         : value( a_value )
         , mode( a_mode )
         , stdKeyword( a_stdKeyword )
      {
      }

      CalibrationParameter() = default;
      CalibrationParameter( const CalibrationParameter& ) = default;
      CalibrationParameter& operator =( const CalibrationParameter& ) = default;

      float operator =( float x )
      {
         return value = x;
      }

      float GetValue( const FITSKeywordArray& keywords ) const
      {
         if ( mode == pcl_enum( FCParameterMode::Literal ) )
            return value;

         for ( const FITSHeaderKeyword& k : keywords )
            if ( mode == pcl_enum( FCParameterMode::StandardKeyword ) && String( k.name ) == stdKeyword ||
                 mode == pcl_enum( FCParameterMode::CustomKeyword ) && String( k.name ) == keyword )
            {
               float retVal;
               if ( k.value.TryToFloat( retVal ) )
                  return retVal;
            }

         return -1;
      }
   };

   CalibrationParameter p_wavelength;            // nm
   CalibrationParameter p_transmissivity;        // (0,1]
   CalibrationParameter p_filterWidth;           // nm
   CalibrationParameter p_aperture;              // mm    *
   CalibrationParameter p_centralObstruction;    // mm    *
   CalibrationParameter p_exposureTime;          // s     *
   CalibrationParameter p_atmosphericExtinction; // [0,1]
   CalibrationParameter p_sensorGain;            //       *
   CalibrationParameter p_quantumEfficiency;     // [0,1)

   friend class FluxCalibrationEngine;
   friend class FluxCalibrationProcess;
   friend class FluxCalibrationInterface;
};

// ----------------------------------------------------------------------------


} // pcl

#endif   // __FluxCalibrationInstance_h

// ----------------------------------------------------------------------------
// EOF FluxCalibrationInstance.h - Released 2021-04-09T19:41:48Z
