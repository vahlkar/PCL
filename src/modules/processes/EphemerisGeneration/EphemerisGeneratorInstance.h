//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard EphemerisGeneration Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// EphemerisGeneratorInstance.h - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard EphemerisGeneration PixInsight module.
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

#ifndef __EphemerisGeneratorInstance_h
#define __EphemerisGeneratorInstance_h

#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>

#include "Elements.h"

namespace pcl
{

// ----------------------------------------------------------------------------

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

private:

   pcl_enum p_workingMode;          // state vectors, elements, or database objects
   Vector   p_position;             // position vector
   Vector   p_velocity;             // velocity vector
   Vector   p_elements;             // osculating orbital elements: a, q, e, M, T, i, O, w
   double   p_epochJD;              // epoch of initial conditions, JD, TDB
   String   p_objectId;             // identifier of the object being integrated
   String   p_objectName;           // name of the object being integrated
   float    p_H;                    // absolute visual magnitude
   float    p_G;                    // slope parameter
   pcl_bool p_B_V_defined;          // B-V value provided?
   float    p_B_V;                  // optional B-V color index (mag)
   pcl_bool p_D_defined;            // D value provided?
   float    p_D;                    // optional object diameter (km)
   String   p_databaseFilePath;     // object database
   String   p_databaseFormatName;   // known database format description
   String   p_objects;              // comma separated list of objects to integrate
   pcl_bool p_useRegularExpressions; // p_objects contains regexps
   pcl_bool p_excludeCoreAsteroids; // don't include asteroids found in core ephemerides
   String   p_fundamentalFilePath;  // fundamental ephemerides
   String   p_asteroidsFilePath;    // asteroid ephemerides
   String   p_KBOsFilePath;         // KBO ephemerides
   double   p_startTimeJD;          // starting time of integration, JD, TDB
   double   p_endTimeJD;            // final time of integration, JD, TDB
   pcl_bool p_useAsteroidPerturbers; // integrate with asteroid perturbers?
   pcl_bool p_useKBOPerturbers;     // integrate with KBO perturbers?
   pcl_bool p_velocityExpansions;   // generate velocity expansions?
   pcl_bool p_separateEarthMoonPerturbers; // individual Earth and Moon perturbers, or combined E-M barycenter
   pcl_bool p_relativisticPerturbations; // include relativistic perturbations (PPN n-body model)
   pcl_bool p_figureEffects;        // include oblateness perturbations
   pcl_bool p_outputXEPHFile;       // whether to generate an ephemeris file
   String   p_outputXEPHFilePath;   // the output XEPH file
   pcl_bool p_overwriteExistingFiles; // whether existing files should be overwritten or preserved
   double   p_denseOutputToleranceFactor; // factor to scale up tolerance for integration dense output generation
   double   p_ephemerisToleranceFactor;   // additional factor to scale up tolerance for ephemeris generation


   friend class Ephemerides;
   template <class, class, class, class> friend class Integration;
   friend class IntegrationThread;
   friend class TextDatabase;
   friend class EphemerisGeneratorProcess;
   friend class EphemerisGeneratorInterface;
};

/*
 * Orbital element parameters
 */
#define el_a   p_elements[a_idx]
#define el_q   p_elements[q_idx]
#define el_e   p_elements[e_idx]
#define el_M   p_elements[M_idx]
#define el_T   p_elements[T_idx]
#define el_i   p_elements[i_idx]
#define el_O   p_elements[O_idx]
#define el_w   p_elements[w_idx]

// ----------------------------------------------------------------------------

} // pcl

#endif   // __EphemerisGeneratorInstance_h

// ----------------------------------------------------------------------------
// EOF EphemerisGeneratorInstance.h - Released 2021-04-09T19:41:48Z
