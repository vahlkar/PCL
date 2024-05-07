//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.11
// ----------------------------------------------------------------------------
// Standard EphemerisGeneration Process Module Version 1.3.0
// ----------------------------------------------------------------------------
// EphemerisGeneratorInstance.h - Released 2024-05-07T15:28:00Z
// ----------------------------------------------------------------------------
// This file is part of the standard EphemerisGeneration PixInsight module.
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

#ifndef __EphemerisGeneratorInstance_h
#define __EphemerisGeneratorInstance_h

#include <pcl/MetaParameter.h>
#include <pcl/OsculatingElements.h>
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Orbital element parameters - vector indexes
 */
#define a_idx  0
#define q_idx  1
#define e_idx  2
#define M_idx  3
#define T_idx  4
#define i_idx  5
#define O_idx  6
#define w_idx  7

/*
 * Orbital element parameters - shorthands
 */
#define el_a   p_elements[a_idx]
#define el_q   p_elements[q_idx]
#define el_e   p_elements[e_idx]
#define el_M   p_elements[M_idx]
#define el_T   p_elements[T_idx]
#define el_i   p_elements[i_idx]
#define el_O   p_elements[O_idx]
#define el_w   p_elements[w_idx]

class EphemerisGeneratorInstance : public ProcessImplementation
{
public:

   EphemerisGeneratorInstance( const MetaProcess* );
   EphemerisGeneratorInstance( const EphemerisGeneratorInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, String& whyNot ) const override;
   bool CanExecuteGlobal( String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

   OsculatingElements ToOsculatingElements() const
   {
      OsculatingElements el;
      el.a = el_a;
      el.q = el_q;
      el.e = el_e;
      el.M = Rad( el_M );
      el.T = el_T;
      el.i = Rad( el_i );
      el.O = Rad( el_O );
      el.w = Rad( el_w );
      return el;
   }

   void FromOsculatingElements( const OsculatingElements& el )
   {
      el_a = el.a;
      el_q = el.q;
      el_e = el.e;
      el_M = Deg( el.M );
      el_T = el.T.JD();
      el_i = Deg( el.i );
      el_O = Deg( el.O );
      el_w = Deg( el.w );
   }

private:

   pcl_enum p_workingMode;             // state vectors, elements, or database objects
   Vector   p_position;                // position vector
   Vector   p_velocity;                // velocity vector
   Vector   p_elements;                // osculating orbital elements: a, q, e, M, T, i, O, w
   double   p_epochJD;                 // epoch of initial conditions, JD, TDB
   String   p_objectId;                // identifier of the object being integrated
   String   p_objectName;              // name of the object being integrated
   pcl_bool p_H_defined;               // H value provided?
   float    p_H;                       // absolute visual magnitude
   pcl_bool p_G_defined;               // G value provided?
   float    p_G;                       // magnitude slope parameter
   pcl_bool p_M1_defined;              // M1 value provided?
   float    p_M1;                      // comet total magnitude
   pcl_bool p_K1_defined;              // K1 value provided?
   float    p_K1;                      // comet total magnitude slope parameter
   pcl_bool p_M2_defined;              // M2 value provided?
   float    p_M2;                      // comet nuclear magnitude
   pcl_bool p_K2_defined;              // K2 value provided?
   float    p_K2;                      // comet nuclear magnitude slope parameter
   pcl_bool p_PC_defined;              // PC value provided?
   float    p_PC;                      // comet nuclear magnitude / phase coefficient
   pcl_bool p_B_V_defined;             // B-V value provided?
   float    p_B_V;                     // B-V color index (mag)
   pcl_bool p_U_B_defined;             // U-B value provided?
   float    p_U_B;                     // U-B color index (mag)
   pcl_bool p_I_R_defined;             // I-R value provided?
   float    p_I_R;                     // I-R color index (mag)
   pcl_bool p_D_defined;               // D value provided?
   float    p_D;                       // optional object diameter (km)
   pcl_bool p_A1_defined;              // A1 value provided?
   float    p_A1;                      // comet non-gravitational acceleration, radial component (au/day^2)
   pcl_bool p_A2_defined;              // A2 value provided?
   float    p_A2;                      // comet non-gravitational acceleration, transverse component (au/day^2)
   pcl_bool p_A3_defined;              // A3 value provided?
   float    p_A3;                      // comet non-gravitational acceleration, normal component (au/day^2)
   pcl_bool p_DT_defined;              // DT value provided?
   float    p_DT;                      // comet non-gravitational acceleration, perihelion time offset (days)
   String   p_databaseFilePath;        // object database
   String   p_databaseFormatName;      // known database format description
   String   p_objects;                 // comma separated list of objects to integrate
   pcl_bool p_useRegularExpressions;   // p_objects contains regexps
   pcl_bool p_excludeCoreAsteroids;    // don't include asteroids found in core ephemerides
   String   p_fundamentalFilePath;     // fundamental ephemerides
   String   p_asteroidsFilePath;       // asteroid ephemerides
   String   p_KBOsFilePath;            // KBO ephemerides
   double   p_startTimeJD;             // starting time of integration, JD, TDB
   double   p_endTimeJD;               // final time of integration, JD, TDB
   pcl_bool p_useAsteroidPerturbers;   // integrate with asteroid perturbers?
   pcl_bool p_useKBOPerturbers;        // integrate with KBO perturbers?
   pcl_bool p_velocityExpansions;      // generate velocity expansions?
   pcl_bool p_separateEarthMoonPerturbers; // individual Earth and Moon perturbers, or combined E-M barycenter
   pcl_bool p_relativisticPerturbations; // include relativistic perturbations (PPN n-body model)
   pcl_bool p_figureEffects;           // include oblateness perturbations
   pcl_bool p_nonGravitationalPerturbations; // include non-gravitational perturbations when available
   pcl_bool p_outputXEPHFile;          // whether to generate an ephemeris file
   String   p_outputXEPHFilePath;      // the output XEPH file
   pcl_bool p_outputLogFile;           // write a log file for multithreaded ephemeris generation
   pcl_bool p_overwriteExistingFiles;  // whether existing files should be overwritten or preserved
   double   p_denseOutputToleranceFactor; // factor to scale up tolerance for integration dense output generation
   double   p_ephemerisToleranceFactor; // additional factor to scale up tolerance for ephemeris generation
   int32    p_ephemerisMaxExpansionLength; // maximum number of coefficients in XEPH Chebyshev expansions
   double   p_ephemerisMaxTruncationError; // maximum truncation error in XEPH Chebyshev expansions

   friend class Ephemerides;
   template <class, class, class, class> friend class Integration;
   friend class EphemerisGenerationEngine;
   friend class IntegrationThread;
   friend class TextDatabase;
   friend class EphemerisGeneratorProcess;
   friend class EphemerisGeneratorInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __EphemerisGeneratorInstance_h

// ----------------------------------------------------------------------------
// EOF EphemerisGeneratorInstance.h - Released 2024-05-07T15:28:00Z
