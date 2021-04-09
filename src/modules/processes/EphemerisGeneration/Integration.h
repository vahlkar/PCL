//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard EphemerisGeneration Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// Integration.h - Released 2021-04-09T19:41:48Z
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

#ifndef __Integration_h
#define __Integration_h

#include "Ephemerides.h"
#include "EphemerisGeneratorInstance.h"
#include "Perturber.h"

#include <pcl/AutoPointer.h>
#include <pcl/Console.h>
#include <pcl/Position.h>

#include <boost/numeric/odeint.hpp>
// #include <boost/multiprecision/float128.hpp>
// #include <boost/multiprecision/cpp_bin_float.hpp>

using namespace boost::numeric::odeint;
// using namespace boost::multiprecision;

#define DEFAULT_SCALAR_TYPE  long double
#define DEFAULT_HSCALAR_TYPE long double

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Dense integration output data.
 */
struct IntegrationDenseOutputItem
{
   double t;       // start of time span (JD)
   double h;       // length of time span (days)
   double eps;     // output tolerance for this integration step (au)
   Vector c0[ 3 ]; // Chebyshev coefficients for position
   Vector c1[ 3 ]; // Chebyshev coefficients for velocity

   bool operator ==( const IntegrationDenseOutputItem& x ) const
   {
      return t == x.t;
   }

   bool operator <( const IntegrationDenseOutputItem& x ) const
   {
      return t < x.t;
   }
};

typedef Array<IntegrationDenseOutputItem>  IntegrationDenseOutputData;

// ----------------------------------------------------------------------------

/*
 * Numerical integration of perturbed n-body orbital motion.
 *
 * The implemented physical model includes:
 *
 * - Perturbations from Mercury, Venus, Earth, Moon, Mars, Jupiter, Saturn,
 *   Uranus, Neptune and Pluto. State vectors and constants are provided by
 *   XEPH fundamental ephemerides (currently DE440 by default).
 *
 * - Optional perturbations from the 343 most massive asteroids used in DE430
 *   and DE440 integrations (enabled by default). State vectors and constants
 *   are provided by XEPH asteroid ephemerides (currently DE430 asteroid
 *   ephemerides consistent with DE440 by default).
 *
 * - Optional perturbations from the 30 most massive Kuiper belt objects
 *   (KBOs). State vectors and constants are provided by XEPH ephemerides
 *   generated with this implementation.
 *
 * - Relativistic perturbation terms (PPN model) up to O(1/c^2) for the
 *   planets, Pluto and the five most massive asteroids.
 *
 * - Direct acceleration perturbation terms caused by oblateness. Zonal
 *   harmonics J2 are computed for Sun, Earth, Mars, Jupiter, Saturn, Uranus
 *   and Neptune.
 *
 * This implementation is intented to compute high-precision ephemerides of
 * asteroids, dwarf planets (including large TNOs and KBOs) and comets.
 */
template <class Scalar_ = DEFAULT_SCALAR_TYPE
        , class HScalar_ = DEFAULT_HSCALAR_TYPE
        , class Vector_ = GenericVector<Scalar_>
        , class HVector_ = GenericVector<HScalar_> >
class Integration
{
public:

   /*
    * Basic type definitions.
    */
   typedef PerturberBase<Scalar_, Vector_>   perturber;
   typedef PerturberBody<Scalar_, Vector_>   perturber_body;
   typedef typename perturber::scalar_type   scalar_type;
   typedef typename perturber::vector_type   vector_type;
   typedef GenericMatrix<scalar_type>        matrix_type;
   typedef scalar_type                       time_type;
   typedef HScalar_                          hscalar_type;
   typedef HVector_                          hvector_type;
   typedef GenericMatrix<hscalar_type>       hmatrix_type;

   /*
    * Integration state vectors compatible with boost::odeint
    */
   class State
   {
   public:

      typedef scalar_type        value_type;
      typedef scalar_type*       iterator;
      typedef const scalar_type* const_iterator;

      State() = default;
      State( const State& ) = default;

      State( const Vector& r, const Vector& v )
      {
         m_y[0] = scalar_type( r[0] ); // position components
         m_y[1] = scalar_type( r[1] );
         m_y[2] = scalar_type( r[2] );
         m_y[3] = scalar_type( v[0] ); // velocity components
         m_y[4] = scalar_type( v[1] );
         m_y[5] = scalar_type( v[2] );
      }

      const scalar_type& operator[]( const size_t i ) const
      {
         return m_y[i];
      }

      scalar_type& operator[]( const size_t i )
      {
         return m_y[i];
      }

      iterator begin()
      {
         return m_y;
      }

      const_iterator begin() const
      {
         return m_y;
      }

      iterator end()
      {
         return m_y + 6;
      }

      const_iterator end() const
      {
         return m_y + 6;
      }

      size_t size() const
      {
         return 6;
      }

      Vector Position() const
      {
         return Vector( m_y[0], m_y[1], m_y[2] );
      }

      Vector Velocity() const
      {
         return Vector( m_y[3], m_y[4], m_y[5] );
      }

   private:

      // Position and velocity components
      scalar_type m_y[ 6 ] = {};
   };

   /*
    * Abstract base class of all integration steppers.
    */
   class Stepper
   {
   public:

      typedef std::function<void ( const State&, State&, const time_type& )>
                        step_func_type;

      Stepper() = default;

      virtual ~Stepper() = default;

      virtual void Initialize( const State& initialState
                             , const time_type& startTime
                             , const time_type& dt ) = 0;

      virtual State CurrentState() const = 0;
      virtual time_type CurrentTime() const = 0;
      virtual time_type CurrentTimeStep() const = 0;

      virtual std::pair<time_type, time_type> DoStep( step_func_type ) = 0;

      virtual void CalcState( const time_type&, State& ) = 0;
   };

   /*
    * Bulirsch-Stoer integration stepper.
    */
   class BulirschStoerStepper : public Stepper
   {
   public:

      typedef typename Stepper::step_func_type  step_func_type;

      typedef bulirsch_stoer_dense_out<State, scalar_type, State, time_type>
                                                stepper_type;

      BulirschStoerStepper( scalar_type epsAbs, scalar_type epsRel )
      {
         m_stepper = new stepper_type( epsAbs, epsRel );
      }

      virtual ~BulirschStoerStepper()
      {
         delete m_stepper;
      }

      void Initialize( const State& initialState
                     , const time_type& startTime
                     , const time_type& dt ) override
      {
         m_stepper->initialize( initialState, startTime, dt );
      }

      State CurrentState() const override
      {
         return m_stepper->current_state();
      }

      time_type CurrentTime() const override
      {
         return m_stepper->current_time();
      }

      time_type CurrentTimeStep() const override
      {
         return m_stepper->current_time_step();
      }

      std::pair<time_type, time_type> DoStep( step_func_type f ) override
      {
         return m_stepper->do_step( f );
      }

      void CalcState( const time_type& t, State& y ) override
      {
         m_stepper->calc_state( t, y );
      }

   private:

      stepper_type* m_stepper = nullptr;
   };

   /*
    * Dormand–Prince (DOPRI) integration stepper.
    */
   class RungeKuttaDOPRI5Stepper : public Stepper
   {
   public:

      typedef typename Stepper::step_func_type  step_func_type;

      typedef runge_kutta_dopri5<State, scalar_type, State, time_type>
                                                rkdopri5_stepper;

      typedef typename result_of::make_dense_output<rkdopri5_stepper>::type
                                                stepper_type;

      RungeKuttaDOPRI5Stepper( scalar_type epsAbs, scalar_type epsRel )
      {
         m_stepper = new stepper_type( make_dense_output( epsAbs, epsRel, rkdopri5_stepper() ) );
      }

      virtual ~RungeKuttaDOPRI5Stepper()
      {
         delete m_stepper;
      }

      void Initialize( const State& initialState
                     , const time_type& startTime
                     , const time_type& dt ) override
      {
         m_stepper->initialize( initialState, startTime, dt );
      }

      State CurrentState() const override
      {
         return m_stepper->current_state();
      }

      time_type CurrentTime() const override
      {
         return m_stepper->current_time();
      }

      time_type CurrentTimeStep() const override
      {
         return m_stepper->current_time_step();
      }

      std::pair<time_type, time_type> DoStep( step_func_type f ) override
      {
         return m_stepper->do_step( f );
      }

      void CalcState( const time_type& t, State& y ) override
      {
         m_stepper->calc_state( t, y );
      }

   private:

      stepper_type* m_stepper = nullptr;
   };

   /*!
    * The exception that we throw when we detect a potential collision between
    * the body being integrated an a perturber body.
    */
   class CollisionException : public Error
   {
   public:

      CollisionException() = default;
      CollisionException( const CollisionException& ) = default;

      CollisionException( const TimePoint& T, const perturber* P )
         : Error( String() << "Collision detected: " << T.ToIsoString() << " | (" << P->Id() << ") " << P->Name() )
      {
      }
   };

   /*
    * Constructor.
    */
   Integration( const Ephemerides& eph
              , const EphemerisGeneratorInstance& instance
              , const IsoString excludeId = IsoString()
              , const IsoString excludeName = IsoString() )
      : m_instance( instance )
   {
      GetC( s_c, s_c2, s_2c2 );

      EphemerisFile& fun = eph.Fundamental();

      /*
       * Planet perturbers
       */
      m_perturbers << new PerturberSun    <Scalar_, Vector_>( fun )
                   << new PerturberMercury<Scalar_, Vector_>( fun )
                   << new PerturberVenus  <Scalar_, Vector_>( fun )
                   << new PerturberMars   <Scalar_, Vector_>( fun )
                   << new PerturberJupiter<Scalar_, Vector_>( fun )
                   << new PerturberSaturn <Scalar_, Vector_>( fun )
                   << new PerturberUranus <Scalar_, Vector_>( fun )
                   << new PerturberNeptune<Scalar_, Vector_>( fun )
                   << new PerturberPluto  <Scalar_, Vector_>( fun );

      /*
       * Earth-Moon perturber(s)
       */
      if ( m_instance.p_separateEarthMoonPerturbers )
      {
         m_perturbers << new PerturberEarth<Scalar_, Vector_>( fun );

         /*
          * The Moon with respect to the solar system barycenter is an optional
          * object in fundamental ephemerides files. If we have it at hand, add
          * it directly to the list of planet perturbers. Otherwise, create a
          * special perturber that synthesizes barycentric Moon coordinates and
          * velocities from Earth-Moon barycenter and Earth state vectors.
          */
         if ( fun.IsObjectAvailable( "Mn", "SSB" ) )
            m_perturbers << new PerturberMoon<Scalar_, Vector_>( fun );
         else
            m_perturbers << new PerturberMoonEMB<Scalar_, Vector_>( fun );
      }
      else
         m_perturbers << new perturber_body( fun, "EMB", fun.ConstantValue( "GMB" ) );

      /*
       * Asteroid perturbers
       */
      if ( m_instance.p_useAsteroidPerturbers )
      {
         EphemerisFile& ast = eph.Asteroids();

         /*
          * Gather all asteroids with known masses. Note that these masses
          * should be coherent with the employed fundamental ephemerides.
          */
         EphemerisObjectList asts = ast.Objects();
         for ( const EphemerisObject& a : asts )
            if ( excludeId.IsEmpty() || a.objectId != excludeId )
               if ( excludeName.IsEmpty() || a.objectName.CompareIC( excludeName ) != 0 )
                  if ( a.originId == "SSB" )
                  {
                     IsoString massCtName = IsoString().Format( "MA%04d", a.objectId.ToInt() );
                     if ( fun.IsConstantAvailable( massCtName ) )
                        m_perturbers << new perturber_body( ast, a.objectId, fun.ConstantValue( massCtName ) );
                  }
      }

      /*
       * KBO perturbers
       */
      if ( m_instance.p_useKBOPerturbers )
      {
         EphemerisFile& tno = eph.KBOs();

         /*
          * Gather all KBOs with known masses. Currently these masses are being
          * taken from DE440.
          */
         EphemerisObjectList tnos = tno.Objects();
         for ( const EphemerisObject& t : tnos )
            if ( excludeId.IsEmpty() || t.objectId != excludeId )
               if ( excludeName.IsEmpty() || t.objectName.CompareIC( excludeName ) != 0 )
                  if ( t.originId == "SSB" )
                  {
                     double m = eph.KBOMass( t );
                     if ( m > 0 )
                        m_perturbers << new perturber_body( tno, t.objectId, m );
                  }
      }

      /*
       * Sort perturbers by increasing order of relative mass, i.e., the Sun
       * will be placed at the end of the list, asteroids at the beginning.
       * This may help to improve accuracy by reducing roundoff errors in long
       * summations of many perturbation correction terms of very different
       * magnitudes.
       */
      m_perturbers.Sort( []( const perturber& a, const perturber& b ) { return a.Gm() < b.Gm(); } );
   }

   /*
    * Virtual destructor.
    */
   virtual ~Integration()
   {
      m_perturbers.Destroy();
   }

   /*
    * Perform numerical integration.
    */
   template <class Monitor_>
   void operator()( const Vector& initialPosition, const Vector& initialVelocity
                  , const TimePoint& startTime, const TimePoint& endTime
                  , Monitor_ monitor )
   {
      m_outputData.Clear();

      /*
       * Integration times.
       */
      m_startTime = startTime;
      m_endTime = endTime;
      time_type timeDelta = time_type( m_endTime - m_startTime );

      /*
       * Initial state vectors.
       */
      State initialState( initialPosition, initialVelocity );
      m_finalState = initialState;

      /*
       * The integration tolerance is a function of the maximum position vector
       * component (in absolute value) and the scalar data type.
       */
      m_eps = Tolerance( initialPosition );

      /*
       * Build an integration stepper with the initial absolute and relative
       * tolerances.
       */
      AutoPointer<Stepper> stepper = NewStepper();

      /*
       * Initialize the integration stepper for the required time span and
       * integration direction.
       */
      stepper->Initialize( initialState
                         , time_type( 0.0 ) /*start_time*/
                         , time_type( (m_startTime < m_endTime) ? +10.0 : -10.0 ) /*dt*/ );

      using boost::numeric::odeint::detail::less_with_sign;
      using boost::numeric::odeint::detail::less_eq_with_sign;

      try
      {
         size_type stepCount = 0;

         while ( less_with_sign( stepper->CurrentTime()
                              , timeDelta
                              , stepper->CurrentTimeStep() ) )
         {
            while ( less_eq_with_sign( time_type( stepper->CurrentTime() + stepper->CurrentTimeStep() )
                                    , timeDelta
                                    , stepper->CurrentTimeStep() ) )
            {
               /*
                * Check if the optimal integration tolerance has changed for
                * the current distance to the center of integration. If this
                * happens, create a new stepper with the new optimal tolerances
                * and continue with current time parameters.
                */
               double eps = Tolerance( stepper->CurrentState().Position() );
               if ( eps != m_eps )
               {
                  m_eps = eps;
                  State y = stepper->CurrentState();
                  time_type t = stepper->CurrentTime();
                  time_type dt = stepper->CurrentTimeStep();
                  stepper = NewStepper();
                  stepper->Initialize( y, t, dt );
               }

               /*
                * Perform an adaptive integration step.
                */
               std::pair<time_type, time_type> ts = stepper->DoStep(
                  [this]( const State& y, State& dydt, const time_type& t )
                  {
                     Motion( y, dydt, t );
                  } );

               /*
                * Perform dense output. Dense output of position and (optional)
                * velocity components is fitted with truncated Chebyshev
                * polynomials at each integration step.
                */
               FitDenseOutput( ts.first, ts.second, stepper.Ptr() );

               /*
                * Perform monitoring.
                */
               monitor( ++stepCount, Abs( ts.second - ts.first ) );
            }

            /*
             * Last step to arrive exactly at the requested end time.
             */
            stepper->Initialize( stepper->CurrentState(), stepper->CurrentTime(), timeDelta - stepper->CurrentTime() );
         }

         m_finalState = stepper->CurrentState();
      }
      catch ( CollisionException& x )
      {
         Console console;
         console.WriteLn(    "<end><cbr><br>"
                             "***" );
         console.CriticalLn( "*** " + x.Message() );
         console.WriteLn(    "*** <br><br>" );
      }
      catch ( ... )
      {
         throw;
      }
   }

   const IntegrationDenseOutputData& OutputData() const
   {
      return m_outputData;
   }

   const State& FinalState() const
   {
      return m_finalState;
   }

private:

         IndirectArray<perturber>    m_perturbers; // the perturber bodies
         TimePoint                   m_startTime;  // starting time of integration
         TimePoint                   m_endTime;    // end time of integration
   const EphemerisGeneratorInstance& m_instance;   // the instance being executed
         scalar_type                 m_eps;        // current integration tolerance (au)
         IntegrationDenseOutputData  m_outputData; // output times and coordinate expansions
         State                       m_finalState; // the state of integration at the requested end time
         hscalar_type                s_c;          // speed of light (au/day) = (s_c_km_s/s_au_km)*86400
         hscalar_type                s_c2;         // square of the speed of light
         hscalar_type                s_2c2;        // 2*s_c2

   constexpr static scalar_type s_au_km  = 149597870.7; // astronomical unit (km)
   constexpr static scalar_type s_c_km_s = 299792.458;  // speed of light (km/s)

   /*
    * PPN perturbed n-body motion
    */
   void Motion( const State& y, State& dydt, const time_type& t ) const
   {
      TimePoint T = m_startTime + double( t );

      /*
       * Velocity components
       */
      dydt[0] = y[3];
      dydt[1] = y[4];
      dydt[2] = y[5];

      /*
       * Position and velocity vectors of the integrated body.
       */
      vector_type Ri( y[0], y[1], y[2] );
      hvector_type Vi( y[3], y[4], y[5] );

      /*
       * Perturber position vectors, distances, and mass factors.
       */
      Array<hvector_type> Rj, Rji, Rij;
      Array<hscalar_type> rij, rij2;
      Array<hscalar_type> mj;
      int n = 0;
      for ( perturber* P : m_perturbers )
      {
         vector_type Rj_;
         P->ComputeState( Rj_, T );
         Rj   << hvector_type( Rj_ );
         hvector_type Rji_ = Rj_ - Ri;
         Rji  << Rji_;
         Rij  << hvector_type( Ri - Rj_ );
         hscalar_type rij2_ = Rji_.Dot( Rji_ );
         rij  << Sqrt( rij2_ );
         rij2 << rij2_;
         mj   << hscalar_type( P->Gm() );
         ++n;
      }

      /*
       * Compute point-mass acceleration.
       */
      hscalar_type vi2_c2 = Vi.Dot( Vi )/s_c2;

      // Acceleration terms
      Array<hvector_type> A;

      for ( int j = 0; j < n; ++j )
      {
         /*
          * Check for a potential collision. We cannot continue integrating
          * when this happens because our physical model ceases to work
          * (infinite acceleration) - just as the body cannot continue in the
          * real world.
          */
         if ( rij[j] - m_perturbers[j]->Re() <= 0 )
            throw CollisionException( T, m_perturbers[j] );

         /*
          * Newtonian point-mass acceleration vector.
          */
         hscalar_type mj_rij3 = mj[j]/rij2[j]/rij[j];
         hvector_type a0 = mj_rij3 * Rji[j];
         A << a0;

         /*
          * Post-Newtonian perturbation correction terms.
          *
          * Urban, Sean E.; Seidelmann, P. K. (2014), Explanatory Supplement to
          * the Astronomical Almanac (3rd Edition), pp. 307-309.
          *
          * Ryan S. Park et al. (2021), The JPL Planetary and Lunar Ephemerides
          * DE440 and DE441. AJ 161 105.
          */
         if ( m_instance.p_relativisticPerturbations )
            if ( m_perturbers[j]->Relativistic() )
            {
               hvector_type Vj = m_perturbers[j]->Velocity( T );
               hvector_type Aj = m_perturbers[j]->Acceleration( T );

               hscalar_type t1 = 0;
               for ( int k = 0; k < n; ++k )
                  if ( m_perturbers[k]->Relativistic() )
                     t1 += mj[k]/rij[k];
               t1 *= 4/s_c2;

               hscalar_type t2 = 0;
               for ( int k = 0; k < n; ++k )
                  if ( k != j )
                     if ( m_perturbers[k]->Relativistic() )
                        t2 += mj[k]/L2Norm( Rj[k] - Rj[j] );
               t2 /= s_c2;

               hscalar_type t3 = vi2_c2;

               hscalar_type t4 = 2*Vj.Dot( Vj )/s_c2;

               hscalar_type t5 = 4*Vi.Dot( Vj )/s_c2;

               hscalar_type t6 = Rij[j].Dot( Vj )/rij[j];
               t6 *= 3*t6/s_2c2;

               hscalar_type t7 = Rji[j].Dot( Aj )/2/s_c2;

               A << -t1*a0
                 << -t2*a0
                 <<  t3*a0
                 <<  t4*a0
                 << -t5*a0
                 << -t6*a0
                 <<  t7*a0
                 <<  ((mj_rij3 * Rij[j].Dot( 4*Vi - 3*Vj )) * (Vi - Vj))/s_c2
                 <<  (7*(mj[j]/rij[j]) * Aj)/s_2c2;
            }

#define _0_  hscalar_type( 0 )
#define _1_  hscalar_type( 1 )

         /*
          * Direct acceleration due to oblateness. These perturbations are only
          * calculated within the harmonic sphere of each oblate perturber.
          *
          * Moyer, Theodore D. (1971), Mathematical Formulation of the
          * Double-Precision Orbit Determination Program (DPODP). JPL Technical
          * Report 32-1527, pp 31-34.
          *
          * Urban, Sean E.; Seidelmann, P. K. (2014), Explanatory Supplement to
          * the Astronomical Almanac (3rd Edition), pp. 309-310.
          */
         if ( m_instance.p_figureEffects )
            if ( m_perturbers[j]->Oblate() )
               if ( m_perturbers[j]->WithinHarmonicSphere( rij[j] ) )
               {
                  /*
                   * North pole and prime meridian coordinates.
                   */
                  scalar_type a0, d0, W;
                  m_perturbers[j]->GetRotation( a0, d0, W, T );

                  /*
                   * Transformation matrices from barycentric equatorial to
                   * body-fixed coordinates.
                   */
                  hscalar_type sa0 = Sin( a0 );
                  hscalar_type ca0 = Cos( a0 );
                  hscalar_type sd0 = Sin( d0 );
                  hscalar_type cd0 = Cos( d0 );
                  hscalar_type sW = Sin( W );
                  hscalar_type cW = Cos( W );
                  hmatrix_type T( -sa0,      ca0,      _0_,
                                  -ca0*sd0, -sa0*sd0,  cd0,
                                   ca0*cd0,  sa0*cd0,  sd0 );
                  hmatrix_type V(  cW,   sW,   _0_,
                                  -sW,   cW,   _0_,
                                   _0_,  _0_,  _1_ );

                  /*
                   * Position vector in body-fixed coordinates.
                   */
                  hvector_type rb = V*T*Rij[j];

                  /*
                   * Transpose of matrix R for transformation from r' to
                   * body-fixed coordinates (Moyer 1971, Eq. 161 - 164).
                   */
                  hscalar_type db = rij[j];
                  hscalar_type n2 = Sqrt( rb[0]*rb[0] + rb[1]*rb[1] );
                  hscalar_type sp = rb[2]/db;
                  hscalar_type cp = n2/db;
                  hscalar_type sl = rb[1]/n2;
                  hscalar_type cl = rb[0]/n2;
                  hmatrix_type R( cp*cl, -sl,  -sp*cl,
                                  cp*sl,  cl,  -sp*sl,
                                  sp,     _0_,  cp );

                  /*
                   * Acceleration vector due to oblateness in body-fixed
                   * coordinates. Here we apply the r' to rb transformation
                   * matrix R in the same expression.
                   */
                  hvector_type a1( R *
                     ((mj[j]
                       * m_perturbers[j]->J2()
                       * m_perturbers[j]->Re()*m_perturbers[j]->Re()/db/db/db/db)
                       * hvector_type( (9*sp*sp - 3)/2,
                                       _0_,
                                       -3*cp*sp )) );
                  /*
                   * Transformation from body-fixed to barycentric equatorial
                   * coordinates.
                   */
                  A << T.Transpose()*V.Transpose()*a1;
               }
      } // for each perturber body

      /*
       * Accumulate acceleration perturbation terms using a second-order
       * iterative Kahan–Babuška algorithm to minimize roundoff errors.
       *
       * Klein, A. (2006), A Generalized Kahan-Babuška Summation Algorithm.
       * Computing 76, 279–293.
       * https://doi.org/10.1007/s00607-005-0139-x
       */
      hvector_type sa( 0.0, 0.0, 0.0 );
      hvector_type c0( 0.0, 0.0, 0.0 );
      hvector_type c1( 0.0, 0.0, 0.0 );
      for ( const hvector_type& a : A )
         for ( int i = 0; i < 3; ++i )
         {
            volatile hscalar_type t = sa[i] + a[i];
            volatile hscalar_type c = (abs( sa[i] ) < abs( a[i] )) ? (a[i] - t) + sa[i] : (sa[i] - t) + a[i];
            sa[i] = t;
            t = c0[i] + c;
            volatile hscalar_type cc = (abs( c0[i] ) < abs( c )) ? (c - t) + c0[i] : (c0[i] - t) + c;
            c0[i] = t;
            c1[i] += cc;
         }

      /*
       * Return acceleration components back to numerical integration.
       */
      dydt[3] = scalar_type( sa[0] + c0[0] + c1[0] );
      dydt[4] = scalar_type( sa[1] + c0[1] + c1[1] );
      dydt[5] = scalar_type( sa[2] + c0[2] + c1[2] );
   }

   /*
    * Generate Chebyshev expansions from interpolated dense integration output.
    *
    * This is an adaptive routine able to split the integration subinterval
    * recursively (following a dyadic scheme). However, no recursion should
    * happen under normal working conditions, maybe except in extreme cases of
    * very fast movement, such as close encounters.
    */
   void FitDenseOutput( const time_type& t0, const time_type& t1, Stepper* stepper, int nrec = 0 )
   {
      const int n = 20;

      // The starting time of this integration step
      time_type t = Min( t0, t1 );
      // The current integration step size
      time_type h = Abs( t1 - t0 );
      // The output tolerance for this integration step
      double eps = OutputTolerance( stepper->CurrentState().Position() );

      /*
       * Fit Chebyshev expansion for position.
       */
      ChebyshevFit T0(
         [&stepper,t]( double dt )
         {
            State y;
            stepper->CalcState( t + dt, y );
            return y.Position();
         },
         0, double( h ), 3, 2*n );

      /*
       * Attempt to truncate expansions to the required relative precision.
       */
      T0.Truncate( eps, 4 );
      if ( unlikely( T0.TruncatedLength() > n ) )
         goto __recurse;

      if ( m_instance.p_velocityExpansions )
      {
         /*
          * If requested, fit Chebyshev expansion for velocity.
          */
         ChebyshevFit T1(
            [&stepper,t]( double dt )
            {
               State y;
               stepper->CalcState( t + dt, y );
               return y.Velocity();
            },
            0, double( h ), 3, 2*n );

         T1.Truncate( eps, 4 );
         if ( unlikely( T1.TruncatedLength() > n ) )
            goto __recurse;

         /*
          * Gather successfully fitted dense output for position and velocity.
          */
// std::cout << "t = " << IsoString().Format( "%+.6f", double( t0 ) )
//           << " s = " << IsoString().Format( "%+.6f", double( t1-t0 ) )
//           << " n0 = " << T0.TruncatedLength()
//           << " n1 = " << T1.TruncatedLength()
//           << " e = " << IsoString().Format( "%.2e", eps )
//           << " nrec = " << nrec
//           << '\n' << std::flush;
         Output( T0, T1, t, eps );
      }
      else
      {
         /*
          * Gather successfully fitted dense output for position only.
          */
// std::cout << "t = " << IsoString().Format( "%+.6f", double( t0 ) )
//           << " s = " << IsoString().Format( "%+.6f", double( t1-t0 ) )
//           << " n0 = " << T0.TruncatedLength()
//           << " e = " << IsoString().Format( "%.2e", eps )
//           << " nrec = " << nrec
//           << '\n' << std::flush;
         Output( T0, t, eps );
      }

      return;

__recurse:

      /*
       * If the expansions cannot be truncated to the required maximum length,
       * perform a new recursion by fitting each half of the integration step
       * separately. Most likely this denotes a problem with integration
       * tolerances, an unexpected stiffness of the system, or some weird issue
       * with auxiliary ephemerides. We are probably going to fail after a few
       * attempts anyway, but don't say we haven't tried.
       */
      if ( h < 0.0001 || nrec == 4 )
         throw Error( String().Format( "Unable to fit integration dense output: t = %.7f, h = %.7f"
                                       , double( t ), double( h ) ) );
      FitDenseOutput( t0, (t0 + t1)/2, stepper, nrec+1 );
      FitDenseOutput( (t0 + t1)/2, t1, stepper, nrec+1 );
   }

   /*
    * Gather dense integration output of time points and Chebyshev polynomial
    * coefficients for position and velocity.
    */
   void Output( const ChebyshevFit& T0, const ChebyshevFit& T1, double t, double eps )
   {
      const ChebyshevFit::coefficient_series c0 = T0.Coefficients();
      const ChebyshevFit::coefficient_series c1 = T1.Coefficients();
      m_outputData << IntegrationDenseOutputItem{ (m_startTime + t).JD()
                                                   , T0.UpperBound()
                                                   , eps
                                                   , { c0[0], c0[1], c0[2] }
                                                   , { c1[0], c1[1], c1[2] } };
   }

   /*
    * Gather dense integration output of time points and Chebyshev polynomial
    * coefficients for position only.
    */
   void Output( const ChebyshevFit& T0, double t, double eps )
   {
      const ChebyshevFit::coefficient_series c0 = T0.Coefficients();
      m_outputData << IntegrationDenseOutputItem{ (m_startTime + t).JD()
                                                   , T0.UpperBound()
                                                   , eps
                                                   , { c0[0], c0[1], c0[2] }
                                                   , { Vector(), Vector(), Vector() } };
   }

   /*
    * Convenience function to calculate the L2-norm of a high-precision vector
    * as an rvalue (i.e. without having to store it as a variable).
    */
   static hscalar_type L2Norm( const hvector_type& r )
   {
      return Sqrt( r.Dot( r ) );
   }

   /*
    * Get high-precision values of constants defining the speed of light in
    * au/day, necessary for relativistic perturbations: c, c^2, 2*c^2.
    *
    * Using these constants with the required precision is important to avoid
    * roundoff errors, especially with long double and arbitrary precision
    * scalar types.
    */
#if 0
   template <typename T>
   static void GetC( T& c, T& c2, T& twoc2 )
   {
      c     = T( "1.7314463267424032927762788003372296661973812438762e2" );
      c2    = T( "2.9979063823897611839267431867162288714742730956045e4" );
      twoc2 = T( "5.9958127647795223678534863734324577429485461912091e4" );
   }
#endif

   static void GetC( double& c, double& c2, double& twoc2 )
   {
      c     = 1.73144632674240329e2;
      c2    = 2.99790638238976118e4;
      twoc2 = 5.99581276477952237e4;
   }

#ifndef _MSC_VER
   static void GetC( long double& c, long double& c2, long double& twoc2 )
   {
      c     = (long double)( 1.7314463267424032928e2L );
      c2    = (long double)( 2.9979063823897611839e4L );
      twoc2 = (long double)( 5.9958127647795223679e4L );
   }
#else
   // Unfortunately, MSVC does not implement the long double type, which
   // considers as equivalent to double.
   static void GetC( long double& c, long double& c2, long double& twoc2 )
   {
      c     = 1.73144632674240329e2;
      c2    = 2.99790638238976118e4;
      twoc2 = 5.99581276477952237e4;
   }
#endif

   /*
    * Create a new stepper with the current integration tolerance.
    */
   Stepper* NewStepper() const
   {
      if ( m_startTime < m_endTime )
         return new BulirschStoerStepper( m_eps/*eps_abs*/, m_eps/*eps_rel*/ );
      return new RungeKuttaDOPRI5Stepper( m_eps/*eps_abs*/, m_eps/*eps_rel*/ );
   }

   /*
    * Optimal integration tolerance in au, as a function of the largest
    * position vector component (in absolute value) and the scalar data type.
    */
   static scalar_type Tolerance( const Vector& r )
   {
      double m = Max( Max( Abs( r[0] ), Abs( r[1] ) ), Abs( r[2] ) );
      return RawTolerance( (scalar_type*)0 ) * Pow10I<scalar_type>()( Max( 0, int( Log( m ) ) ) );
   }

   /*
    * Tolerance for dense output generation in au.
    */
   double OutputTolerance( const Vector& r ) const
   {
      double m = Max( Max( Abs( r[0] ), Abs( r[1] ) ), Abs( r[2] ) );
      return Max( scalar_type( 1.0e-15 ), RawTolerance( (scalar_type*)0 ) )
           * Max( 1.0, m )
           * m_instance.p_denseOutputToleranceFactor;
   }

   /*
    * Raw integration tolerances for different scalar types.
    */
   static double RawTolerance( double* )
   {
      return 1.0e-15;
   }

#ifndef _MSC_VER
   static long double RawTolerance( long double* )
   {
      return 1.0e-16;
   }
#else
   // See comment about MSVC and long double above.
   static long double RawTolerance( long double* )
   {
      return 1.0e-15;
   }
#endif

#if 0
   static float128 RawTolerance( float128* )
   {
      return float128( "1.0e-24" );
   }
   static cpp_bin_float_quad RawTolerance( cpp_bin_float_quad* )
   {
      return cpp_bin_float_quad( "1.0e-24" );
   }
#endif
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __Integration_h

// ----------------------------------------------------------------------------
// EOF Integration.h - Released 2021-04-09T19:41:48Z
