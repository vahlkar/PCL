//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.7.0
// ----------------------------------------------------------------------------
// Standard EphemerisGeneration Process Module Version 1.3.0
// ----------------------------------------------------------------------------
// Perturber.h - Released 2024-06-18T15:49:25Z
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

#ifndef __Perturber_h
#define __Perturber_h

#include <pcl/EphemerisFile.h>
#include <pcl/Position.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Abstract base class of all perturber body classes.
 */
template <class Scalar_ = long double
        , class Vector_ = GenericVector<Scalar_>
        , class Matrix_ = GenericMatrix<Scalar_> >
class PerturberBase
{
public:

   using scalar_type = Scalar_;
   using vector_type = Vector_;
   using matrix_type = Matrix_;

   PerturberBase( double Gm, double J2, double Rh, double Rm, double Re = 0, double Rp = 0 )
      : m_Gm( Gm )
      , m_J2( J2 )
      , m_Rh( Rh/149597870.7 )
      , m_relativistic( Gm > 1.0e-14 )
      , m_oblate( J2 > 0 )
      , m_Rm( Rm/149597870.7 )
      , m_Re( Re/149597870.7 )
      , m_Rp( Rp/149597870.7 )
   {
      if ( m_Re == 0 )
         m_Re = m_Rm;
      if ( m_Rp == 0 )
         m_Rp = m_Rm;
   }

   virtual ~PerturberBase() = default;

   virtual IsoString Id() const = 0;
   virtual IsoString Name() const = 0;

   virtual void ComputeState( vector_type&, const TimePoint& ) const = 0;
   virtual void ComputeState( vector_type&, vector_type&, const TimePoint& ) const = 0;
   virtual vector_type Velocity( const TimePoint& ) const = 0;
   virtual vector_type Acceleration( const TimePoint& ) const = 0;
   virtual double TrueDistance( const TimePoint& ) const = 0;
   virtual double LightTravelTime( const TimePoint& ) const = 0;

   /*
    * Compute the direction of the north pole of rotation and the prime
    * meridian of the body.
    */
   virtual void GetRotation( scalar_type& a0, scalar_type& d0, scalar_type& W, const TimePoint& ) const
   {
      a0 = scalar_type( 0 );
      d0 = scalar_type( Pi()/2 );
      W = scalar_type( 0 );
   }

   /*
    * Matrix for transformation from ICRS barycentric equatorial to body-fixed
    * coordinates.
    */
   matrix_type ToBodyFixedMatrix( const TimePoint& T ) const
   {
      scalar_type a0, d0, W;
      GetRotation( a0, d0, W, T );
      scalar_type sa = Sin( a0 );
      scalar_type ca = Cos( a0 );
      scalar_type sd = Sin( d0 );
      scalar_type cd = Cos( d0 );
      scalar_type sW = Sin( W );
      scalar_type cW = Cos( W );
//    return matrix_type(  cW,  sW, _0_,
//                        -sW,  cW, _0_,
//                        _0_, _0_, _1_ )
//         * matrix_type( _1_, _0_, _0_,
//                        _0_,  sd,  cd,
//                        _0_, -cd,  sd )
//         * matrix_type( -sa,  ca, _0_,
//                        -ca, -sa, _0_,
//                        _0_, _0_, _1_ );
      return matrix_type( -cW*sa - ca*sd*sW,  ca*cW - sa*sd*sW, cd*sW,
                          -ca*cW*sd + sa*sW, -cW*sa*sd - ca*sW, cd*cW,
                           ca*cd,             cd*sa,            sd );
   }

   /*
    * Gravitational parameter: the gravitational constant multiplied by the
    * relative mass of the body.
    */
   constexpr scalar_type Gm() const
   {
      return m_Gm;
   }

   /*
    * J2 zonal harmonic coefficient.
    */
   constexpr scalar_type J2() const
   {
      return m_J2;
   }

   /*
    * Mean radius of body in au.
    */
   constexpr scalar_type Rm() const
   {
      return m_Rm;
   }

   /*
    * Equatorial radius of body in au.
    */
   constexpr scalar_type Re() const
   {
      return m_Re;
   }

   /*
    * Polar radius of body in au.
    */
   constexpr scalar_type Rp() const
   {
      return m_Rp;
   }

   /*
    * Returns true iff this body should be used to calculate relativistic
    * perturbations. This depends on the mass of the body.
    */
   constexpr bool Relativistic() const
   {
      return m_relativistic;
   }

   /*
    * Returns true iff this body should be used to calculate perturbations due
    * to its oblateness or non-sphericity. This depends on whether zonal
    * harmonic coefficients have been defined.
    */
   constexpr bool Oblate() const
   {
      return m_oblate;
   }

   /*
    * Returns true iff a body at the specified distance 'r' is within the
    * harmonic sphere of this perturber. In this implementation, the harmonic
    * sphere is roughly twice the sphere of influence, where the perturber body
    * has the largest gravitational influence. Perturbations due to oblateness
    * are only calculated within the harmonic sphere of each perturber body.
    */
   template <typename T>
   constexpr bool WithinHarmonicSphere( const T& r ) const
   {
      return r < m_Rh;
   }

private:

   scalar_type m_Gm;           // G * mass of the body
   scalar_type m_J2;           // J2 zonal harmonic coefficient
   scalar_type m_Rh;           // harmonic sphere radius (au)
   bool        m_relativistic; // whether to compute relativistic point-mass acceleration
   bool        m_oblate;       // whether to compute perturbations due to oblateness

protected:

   scalar_type m_Rm; // mean radius in au
   scalar_type m_Re; // equatorial radius in au
   scalar_type m_Rp; // polar radius in au
};

// ----------------------------------------------------------------------------

/*
 * Perturber body from fundamental and asteroid ephemerides.
 */
template <class Scalar_ = long double
        , class Vector_ = GenericVector<Scalar_> >
class PerturberBody : public PerturberBase<Scalar_, Vector_>
{
public:

   using perturber_base = PerturberBase<Scalar_, Vector_>;
   using scalar_type = typename perturber_base::scalar_type;
   using vector_type = typename perturber_base::vector_type;

   PerturberBody( const EphemerisFile& eph, const IsoString& id, double Gm,
                  double J2 = 0, double Rh = 0, double Rm = 0, double Re = 0, double Rp = 0 )
      : perturber_base( Gm, J2, Rh, Rm, Re, Rp )
      , m_handle( eph, id, "SSB" )
   {
      if ( this->m_Rm == 0 )
      {
         if ( m_handle.D().IsDefined() )
            this->m_Rm = m_handle.D()/149597870.7/2;
         else
            this->m_Rm = 75.0/149597870.7;
         this->m_Re = this->m_Rp = this->m_Rm;
      }
   }

   IsoString Id() const override
   {
      return m_handle.ObjectId();
   }

   IsoString Name() const override
   {
      return m_handle.ObjectName().ToIsoString();
   }

   void ComputeState( vector_type& p, const TimePoint& t ) const override
   {
      Vector p_;
      m_handle.ComputeState( p_, t );
      p = vector_type( p_ );
   }

   void ComputeState( vector_type& p, vector_type& v, const TimePoint& t ) const override
   {
      Vector p_, v_;
      m_handle.ComputeState( p_, v_, t );
      p = vector_type( p_ );
      v = vector_type( v_ );
   }

   vector_type Velocity( const TimePoint& t ) const override
   {
      return vector_type( m_handle.FirstDerivative( t ) );
   }

   vector_type Acceleration( const TimePoint& t ) const override
   {
      return vector_type( m_handle.SecondDerivative( t ) );
   }

   double TrueDistance( const TimePoint& t ) const override
   {
      return Position( t, "TDB" ).TrueDistance( m_handle );
   }

   double LightTravelTime( const TimePoint& t ) const override
   {
      return Position( t, "TDB" ).LightTravelTime( m_handle );
   }

private:

   mutable EphemerisFile::Handle m_handle;
};

// ----------------------------------------------------------------------------

template <class Scalar_ = long double
        , class Vector_ = GenericVector<Scalar_> >
class PerturberSun : public PerturberBody<Scalar_, Vector_>
{
public:

   using perturber_body = PerturberBody<Scalar_, Vector_>;
   using scalar_type = typename perturber_body::scalar_type;
   using vector_type = typename perturber_body::vector_type;

   PerturberSun( const EphemerisFile& eph )
      : perturber_body( eph, "Sn"
                      , eph.ConstantValue( "GMS" )
                      , eph.ConstantValue( "J2SUN" )
                      , 8.0e+13/*Rh*/
                      , 695700.0/*Re*/ )
   {
   }

   void GetRotation( scalar_type& a0, scalar_type& d0, scalar_type& W, const TimePoint& t ) const override
   {
      /*
       * B. A. Archinal et al. (2018) Report of the IAU Working Group on
       * Cartographic Coordinates and Rotational Elements: 2015.
       * https://doi.org/10.1007/s10569-017-9805-5
       */
      a0 = Rad( 286.13 );
      d0 = Rad( 63.87 );
      W = Norm2Pi( Rad( 84.176 + 14.1844000*t.DaysSinceJ2000() ) );
   }
};

// ----------------------------------------------------------------------------

template <class Scalar_ = long double
        , class Vector_ = GenericVector<Scalar_> >
class PerturberMercury : public PerturberBody<Scalar_, Vector_>
{
public:

   using perturber_body = PerturberBody<Scalar_, Vector_>;
   using scalar_type = typename perturber_body::scalar_type;
   using vector_type = typename perturber_body::vector_type;

   PerturberMercury( const EphemerisFile& eph )
      : perturber_body( eph, "Me"
                      , eph.ConstantValue( "GM1" )
                      , 0.0/*J2*/
                      , 0.0/*Rh*/
                      , 2439.4/*Rm*/
                      , 2440.53/*Re*/
                      , 2438.26/*Rp*/ )
   {
   }

   void GetRotation( scalar_type& a0, scalar_type& d0, scalar_type& W, const TimePoint& t ) const override
   {
      /*
       * B. A. Archinal et al. (2018) Report of the IAU Working Group on
       * Cartographic Coordinates and Rotational Elements: 2015.
       * https://doi.org/10.1007/s10569-017-9805-5
       */
      double T = t.CenturiesSinceJ2000();
      double d = t.DaysSinceJ2000();
      a0 = Norm2Pi( Rad( 281.0103 - 0.0328*T ) );
      d0 = Rad( 61.4155 - 0.0049*T );
      W = Norm2Pi( Rad( 329.5988 + 6.1385108*d
            + 0.01067257*Sin( Rad( 174.7910857 +  4.092335*d ) )
            - 0.00112309*Sin( Rad( 349.5821714 +  8.184670*d ) )
            - 0.00011040*Sin( Rad( 164.3732571 + 12.277005*d ) )
            - 0.00002539*Sin( Rad( 339.1643429 + 16.369340*d ) )
            - 0.00000571*Sin( Rad( 153.9554286 + 20.461675*d ) ) ) );
   }
};

// ----------------------------------------------------------------------------

template <class Scalar_ = long double
        , class Vector_ = GenericVector<Scalar_> >
class PerturberVenus : public PerturberBody<Scalar_, Vector_>
{
public:

   using perturber_body = PerturberBody<Scalar_, Vector_>;
   using scalar_type = typename perturber_body::scalar_type;
   using vector_type = typename perturber_body::vector_type;

   PerturberVenus( const EphemerisFile& eph )
      : perturber_body( eph, "Ve"
                      , eph.ConstantValue( "GM2" )
                      , 0.0/*J2*/
                      , 0.0/*Rh*/
                      , 6051.8/*Rm*/ )
   {
   }

   void GetRotation( scalar_type& a0, scalar_type& d0, scalar_type& W, const TimePoint& t ) const override
   {
      /*
       * B. A. Archinal et al. (2018) Report of the IAU Working Group on
       * Cartographic Coordinates and Rotational Elements: 2015.
       * https://doi.org/10.1007/s10569-017-9805-5
       */
      a0 = Rad( 272.76 );
      d0 = Rad( 67.16 );
      W = Norm2Pi( Rad( 160.20 - 1.4813688*t.DaysSinceJ2000() ) );
   }
};

// ----------------------------------------------------------------------------

template <class Scalar_ = long double
        , class Vector_ = GenericVector<Scalar_> >
class PerturberEarth : public PerturberBody<Scalar_, Vector_>
{
public:

   using perturber_body = PerturberBody<Scalar_, Vector_>;
   using scalar_type = typename perturber_body::scalar_type;
   using vector_type = typename perturber_body::vector_type;

   PerturberEarth( const EphemerisFile& eph )
      : perturber_body( eph, "Ea"
                      , eph.ConstantValue( "GMB" )/(1 + 1/eph.ConstantValue( "EMRAT" ))
                      , eph.ConstantValue( "J2E" )
                      , 2.0e+6/*Rh*/
                      , 6371.0084/*Rm*/
                      , 6378.1366/*Re*/
                      , 6356.7519/*Rp*/ )
   {
   }

   void GetRotation( scalar_type& a0, scalar_type& d0, scalar_type& W, const TimePoint& t ) const override
   {
      /*
       * Urban, Sean E.; Seidelmann, P. K. (2014), Explanatory Supplement to
       * the Astronomical Almanac (3rd Edition), Table 10.1.
       *
       * P. K. Seidelmann et al. (2002), Report of the IAU/IAG Working Group on
       * Cartographic Coordinates and Rotational Elements of the Planets and
       * Satellites: 2000, Celestial Mechanics and Dynamical Astronomy 82:
       * 83-110
       *
       * N.B. These are approximate equations.
       * ### TODO - Use IERS standards instead.
       */
      double T = t.CenturiesSinceJ2000();
      a0 = Norm2Pi( Rad( -0.641*T ) );
      d0 = Rad( 90 - 0.557*T );
      W = Norm2Pi( Rad( 190.147 + 360.9856235*t.DaysSinceJ2000() ) );
   }
};

// ----------------------------------------------------------------------------

template <class Scalar_ = long double
        , class Vector_ = GenericVector<Scalar_> >
class PerturberMars : public PerturberBody<Scalar_, Vector_>
{
public:

   using perturber_body = PerturberBody<Scalar_, Vector_>;
   using scalar_type = typename perturber_body::scalar_type;
   using vector_type = typename perturber_body::vector_type;

   PerturberMars( const EphemerisFile& eph )
      : perturber_body( eph, "Ma"
                      , eph.ConstantValue( "GM4" )
                      , 1.9555e-3/*J2*/
                      , 1.2e+6/*Rh*/
                      , 3389.50/*Rm*/
                      , 3396.19/*Re*/
                      , 3376.20/*Rp*/ )
   {
      /*
       * J2 value taken from:
       * Xiaodong Liu et al. (2011), Extension of the critical inclination,
       * Astrophysics and Space Science
       * https://doi.org/10.1007/s10509-011-0685-y
       */
   }

   void GetRotation( scalar_type& a0, scalar_type& d0, scalar_type& W, const TimePoint& t ) const override
   {
      /*
       * B. A. Archinal et al. (2018) Report of the IAU Working Group on
       * Cartographic Coordinates and Rotational Elements: 2015.
       * https://doi.org/10.1007/s10569-017-9805-5
       */
      double T = t.CenturiesSinceJ2000();
      a0 = Norm2Pi( Rad( 317.269202 - 0.10927547*T
               + 0.000068*Sin( Rad( 198.991226 + 19139.4819985*T ) )
               + 0.000238*Sin( Rad( 226.292679 + 38280.8511281*T ) )
               + 0.000052*Sin( Rad( 249.663391 + 57420.7251593*T ) )
               + 0.000009*Sin( Rad( 266.183510 + 76560.6367950*T ) )
               + 0.419057*Sin( Rad(  79.398797 +     0.5042615*T ) ) ) );
      d0 = Rad( 54.432516 - 0.05827105*T
               + 0.000051*Cos( Rad( 122.433576 + 19139.9407476*T ) )
               + 0.000141*Cos( Rad(  43.058401 + 38280.8753272*T ) )
               + 0.000031*Cos( Rad(  57.663379 + 57420.7517205*T ) )
               + 0.000005*Cos( Rad(  79.476401 + 76560.6495004*T ) )
               + 1.591274*Cos( Rad( 166.325722 +     0.5042615*T ) ) );
      W = Norm2Pi( Rad( 176.049863 + 350.891982443297*t.DaysSinceJ2000()
               + 0.000145*Sin( Rad( 129.071773 + 19140.0328244*T ) )
               + 0.000157*Sin( Rad(  36.352167 + 38281.0473591*T ) )
               + 0.000040*Sin( Rad(  56.668646 + 57420.9295360*T ) )
               + 0.000001*Sin( Rad(  67.364003 + 76560.2552215*T ) )
               + 0.000001*Sin( Rad( 104.792680 + 95700.4387578*T ) )
               + 0.584542*Sin( Rad(  95.391654 +     0.5042615*T ) ) ) );
   }
};

// ----------------------------------------------------------------------------

template <class Scalar_ = long double
        , class Vector_ = GenericVector<Scalar_> >
class PerturberJupiter : public PerturberBody<Scalar_, Vector_>
{
public:

   using perturber_body = PerturberBody<Scalar_, Vector_>;
   using scalar_type = typename perturber_body::scalar_type;
   using vector_type = typename perturber_body::vector_type;

   PerturberJupiter( const EphemerisFile& eph )
      : perturber_body( eph, "Ju"
                      , eph.ConstantValue( "GM5" )
                      , 14696.514e-6/*J2*/
                      , 100e+6/*Rh*/
                      , 69911.0/*Rm*/
                      , 71492.0/*Re*/
                      , 66854.0/*Rp*/ )
   {
      /*
       * J2 value:
       *
       * Folkner, W. M., et al. (2017), Jupiter gravity field estimated from
       * the first two Juno orbits, Geophys. Res. Lett., 44, 4694-4700
       * https://doi.org/10.1002/2017GL073140
       */
   }

   void GetRotation( scalar_type& a0, scalar_type& d0, scalar_type& W, const TimePoint& t ) const override
   {
      /*
       * B. A. Archinal et al. (2018) Report of the IAU Working Group on
       * Cartographic Coordinates and Rotational Elements: 2015.
       * https://doi.org/10.1007/s10569-017-9805-5
       */
      double T = t.CenturiesSinceJ2000();
      double Ja = Norm2Pi( Rad( 99.360714 + 4850.4046*T ) );
      double Jb = Norm2Pi( Rad( 175.895369 + 1191.9605*T ) );
      double Jc = Norm2Pi( Rad( 300.323162 + 262.5475*T ) );
      double Jd = Norm2Pi( Rad( 114.012305 + 6070.2476*T ) );
      double Je = Norm2Pi( Rad( 49.511251 + 64.3000*T ) );
      a0 = Norm2Pi( Rad( 268.056595 - 0.006499*T
               + 0.000117*Sin( Ja ) + 0.000938*Sin( Jb )
               + 0.001432*Sin( Jc ) + 0.000030*Sin( Jd ) + 0.002150*Sin( Je ) ) );
      d0 = Rad( 64.495303 + 0.002413*T
               + 0.000050*Cos( Ja ) + 0.000404*Cos( Jb )
               + 0.000617*Cos( Jc ) - 0.000013*Cos( Jd ) + 0.000926*Cos( Je ) );
      W = Norm2Pi( Rad( 284.95 + 870.5360000*t.DaysSinceJ2000() ) );
   }
};

// ----------------------------------------------------------------------------

template <class Scalar_ = long double
        , class Vector_ = GenericVector<Scalar_> >
class PerturberSaturn : public PerturberBody<Scalar_, Vector_>
{
public:

   using perturber_body = PerturberBody<Scalar_, Vector_>;
   using scalar_type = typename perturber_body::scalar_type;
   using vector_type = typename perturber_body::vector_type;

   PerturberSaturn( const EphemerisFile& eph )
      : perturber_body( eph, "Sa"
                      , eph.ConstantValue( "GM6" )
                      , 16290.573e-6/*J2*/
                      , 110e+6/*Rh*/
                      , 58232.0/*Rm*/
                      , 60268.0/*Re*/
                      , 54364.0/*Rp*/ )
   {
      /*
       * J2 value:
       *
       * L. Iess et al. (2019) Measurement and implications of Saturn's gravity
       * field and ring mass, Science 364
       * http://dx.doi.org/10.1126/science.aat2965
       */
   }

   void GetRotation( scalar_type& a0, scalar_type& d0, scalar_type& W, const TimePoint& t ) const override
   {
      /*
       * B.A. Archinal et al. (2018) Report of the IAU Working Group on
       * Cartographic Coordinates and Rotational Elements: 2015.
       * https://doi.org/10.1007/s10569-017-9805-5
       */
      double T = t.CenturiesSinceJ2000();
      a0 = Rad( 40.589 - 0.036*T );
      d0 = Rad( 83.537 - 0.004*T );
      W = Norm2Pi( Rad( 38.90 + 810.7939024*t.DaysSinceJ2000() ) );
   }
};

// ----------------------------------------------------------------------------

template <class Scalar_ = long double
        , class Vector_ = GenericVector<Scalar_> >
class PerturberUranus : public PerturberBody<Scalar_, Vector_>
{
public:

   using perturber_body = PerturberBody<Scalar_, Vector_>;
   using scalar_type = typename perturber_body::scalar_type;
   using vector_type = typename perturber_body::vector_type;

   PerturberUranus( const EphemerisFile& eph )
      : perturber_body( eph, "Ur"
                      , eph.ConstantValue( "GM7" )
                      , 3341.29e-6/*J2*/
                      , 100e+6/*Rh*/
                      , 25362.0/*Rm*/
                      , 25559.0/*Re*/
                      , 24973.0/*Rp*/ )
   {
      /*
       * J2 value:
       *
       * M. Podolak and R. Helled (2012) What do we really know about Uranus
       * and Neptune?, The Astrophysical Journal Letters, 759:L32
       * https://doi.org/10.1088/2041-8205/759/2/L32
       */
   }

   void GetRotation( scalar_type& a0, scalar_type& d0, scalar_type& W, const TimePoint& t ) const override
   {
      /*
       * B. A. Archinal et al. (2018) Report of the IAU Working Group on
       * Cartographic Coordinates and Rotational Elements: 2015.
       * https://doi.org/10.1007/s10569-017-9805-5
       */
      a0 = Rad( 257.311 );
      d0 = Rad( -15.175 );
      W = Norm2Pi( Rad( 203.81 - 501.1600928*t.DaysSinceJ2000() ) );
   }
};

// ----------------------------------------------------------------------------

template <class Scalar_ = long double
        , class Vector_ = GenericVector<Scalar_> >
class PerturberNeptune : public PerturberBody<Scalar_, Vector_>
{
public:

   using perturber_body = PerturberBody<Scalar_, Vector_>;
   using scalar_type = typename perturber_body::scalar_type;
   using vector_type = typename perturber_body::vector_type;

   PerturberNeptune( const EphemerisFile& eph )
      : perturber_body( eph, "Ne"
                      , eph.ConstantValue( "GM8" )
                      , 3408.43e-6/*J2*/
                      , 180e+6/*Rh*/
                      , 24622.0/*Rm*/
                      , 24764.0/*Re*/
                      , 24341.0/*Rp*/ )
   {
      /*
       * J2 value:
       *
       * M. Podolak and R. Helled (2012) What do we really know about Neptune
       * and Neptune?, The Astrophysical Journal Letters, 759:L32
       * https://doi.org/10.1088/2041-8205/759/2/L32
       */
   }

   void GetRotation( scalar_type& a0, scalar_type& d0, scalar_type& W, const TimePoint& t ) const override
   {
      /*
       * B. A. Archinal et al. (2018) Report of the IAU Working Group on
       * Cartographic Coordinates and Rotational Elements: 2015.
       * https://doi.org/10.1007/s10569-017-9805-5
       */
      double N = Rad( 357.85 + 52.316*t.CenturiesSinceJ2000() );
      double sN = Sin( N );
      double cN = Cos( N );
      a0 = Rad( 299.36 + 0.70*sN );
      d0 = Rad(  43.46 - 0.51*cN );
      W = Norm2Pi( Rad( 249.978 + 541.1397757*t.DaysSinceJ2000() - 0.48*sN ) );
   }
};

// ----------------------------------------------------------------------------

template <class Scalar_ = long double
        , class Vector_ = GenericVector<Scalar_> >
class PerturberPluto : public PerturberBody<Scalar_, Vector_>
{
public:

   using perturber_body = PerturberBody<Scalar_, Vector_>;
   using scalar_type = typename perturber_body::scalar_type;
   using vector_type = typename perturber_body::vector_type;

   PerturberPluto( const EphemerisFile& eph )
      : perturber_body( eph, "Pl"
                      , eph.ConstantValue( "GM9" )
                      , 0.0/*J2*/
                      , 0.0/*Rh*/
                      , 1195.0/*Rm*/ )
   {
   }

   void GetRotation( scalar_type& a0, scalar_type& d0, scalar_type& W, const TimePoint& t ) const override
   {
      /*
       * B. A. Archinal et al. (2018) Report of the IAU Working Group on
       * Cartographic Coordinates and Rotational Elements: 2015.
       * https://doi.org/10.1007/s10569-017-9805-5
       */
      a0 = Rad( 132.993 );
      d0 = Rad( -6.163 );
      W = Norm2Pi( Rad( 302.695 + 56.3625225*t.DaysSinceJ2000() ) );
   }
};

// ----------------------------------------------------------------------------

template <class scalar_type>
inline void GetMoonRotation( scalar_type& a0, scalar_type& d0, scalar_type& W, const TimePoint& t )
{
   /*
    * R. B. Roncoli (2005), Lunar Constants and Models Document, Technical
    * Report D-32296 (Jet Propulsion Laboratory).
    */
   double T = t.CenturiesSinceJ2000();
   double D = t.DaysSinceJ2000();
   double E1  = Norm2Pi( Rad( 125.045 -  0.0529921*D ) );
   double E2  = Norm2Pi( Rad( 250.089 -  0.1059842*D ) );
   double E3  = Norm2Pi( Rad( 260.008 + 13.0120009*D ) );
   double E4  = Norm2Pi( Rad( 176.625 + 13.3407154*D ) );
   double E5  = Norm2Pi( Rad( 357.529 +  0.9856003*D ) );
   double E6  = Norm2Pi( Rad( 311.589 + 26.4057084*D ) );
   double E7  = Norm2Pi( Rad( 134.963 + 13.0649930*D ) );
   double E8  = Norm2Pi( Rad( 276.617 +  0.3287146*D ) );
   double E9  = Norm2Pi( Rad(  34.226 +  1.7484877*D ) );
   double E10 = Norm2Pi( Rad(  15.134 -  0.1589763*D ) );
   double E11 = Norm2Pi( Rad( 119.743 +  0.0036096*D ) );
   double E12 = Norm2Pi( Rad( 239.961 +  0.1643573*D ) );
   double E13 = Norm2Pi( Rad(  25.053 + 12.9590088*D ) );

   double sinE1  = Sin( E1 );
   double cosE1  = Cos( E1 );
   double sinE2  = Sin( E2 );
   double cosE2  = Cos( E2 );
   double sinE3  = Sin( E3 );
   double cosE3  = Cos( E3 );
   double sinE4  = Sin( E4 );
   double cosE4  = Cos( E4 );
   double sinE5  = Sin( E5 );
   double sinE6  = Sin( E6 );
   double cosE6  = Cos( E6 );
   double sinE7  = Sin( E7 );
   double cosE7  = Cos( E7 );
   double sinE8  = Sin( E8 );
   double sinE9  = Sin( E9 );
   double sinE10 = Sin( E10 );
   double cosE10 = Cos( E10 );
   double sinE11 = Sin( E11 );
   double sinE12 = Sin( E12 );
   double sinE13 = Sin( E13 );
   double cosE13 = Cos( E13 );

   a0 = Norm2Pi( Rad( 269.9949 + 0.0031*T - 3.8787*sinE1 - 0.1204*sinE2 + 0.0700*sinE3
                  - 0.0172*sinE4 + 0.0072*sinE6 - 0.0052*sinE10 + 0.0043*sinE13 ) );

   d0 = Rad( 66.5392 + 0.0130*T + 1.5419*cosE1 + 0.0239*cosE2 - 0.0278*cosE3
                  + 0.0068*cosE4 - 0.0029*cosE6 + 0.0009*cosE7 + 0.0008*cosE10 - 0.0009*cosE13 );

   W = Norm2Pi( Rad( 38.3213 + 13.17635815*D - 1.4e-12*D*D
                  + 3.5610*sinE1 + 0.1208*sinE2 - 0.0642*sinE3 + 0.0158*sinE4 + 0.0252*sinE5
                  - 0.0066*sinE6 - 0.0047*sinE7 - 0.0046*sinE8 + 0.0028*sinE9 + 0.0052*sinE10
                  + 0.0040*sinE11 + 0.0019*sinE12 - 0.0044*sinE13 ) );
}

// ----------------------------------------------------------------------------

template <class Scalar_ = long double
        , class Vector_ = GenericVector<Scalar_> >
class PerturberMoon : public PerturberBody<Scalar_, Vector_>
{
public:

   using perturber_body = PerturberBody<Scalar_, Vector_>;
   using scalar_type = typename perturber_body::scalar_type;
   using vector_type = typename perturber_body::vector_type;

   PerturberMoon( const EphemerisFile& eph )
      : perturber_body( eph, "Mn"
                      , eph.ConstantValue( "GMB" )/(1 + eph.ConstantValue( "EMRAT" ))
                      , eph.ConstantValue( "J2M" )
                      , 2.0e+5/*Rh*/
                      , 1737.4/*Rm*/ )
   {
      /*
       * Alternative J2 value (unnormalized, adjusted for permanent tide):
       * 2.0335425e-4
       *
       * R. B. Roncoli (2005), Lunar Constants and Models Document, Technical
       * Report D-32296 (Jet Propulsion Laboratory).
       */
   }

   void GetRotation( scalar_type& a0, scalar_type& d0, scalar_type& W, const TimePoint& t ) const override
   {
      GetMoonRotation( a0, d0, W, t );
   }
};

// ----------------------------------------------------------------------------

/*
 * Special Moon perturber to synthesize barycentric position and velocity
 * vectors from geocentric Moon and Earth-Moon barycenter vectors.
 */
template <class Scalar_ = long double
        , class Vector_ = GenericVector<Scalar_> >
class PerturberMoonEMB : public PerturberBase<Scalar_, Vector_>
{
public:

   using perturber_base = PerturberBase<Scalar_, Vector_>;
   using scalar_type = typename perturber_base::scalar_type;
   using vector_type = typename perturber_base::vector_type;

   PerturberMoonEMB( const EphemerisFile& eph )
      : perturber_base( eph.ConstantValue( "GMB" )/(1 + eph.ConstantValue( "EMRAT" ))
                      , eph.ConstantValue( "J2M" )
                      , 2.0e+5/*Rh*/
                      , 1737.4/*Rm*/ )
      , m_embHandle( eph, "EMB" )
      , m_moonHandle( eph, "Mn" )
      , m_emb2Earth( 1/(1 + eph.ConstantValue( "EMRAT" )) )
   {
   }

   IsoString Id() const override
   {
      return "Mn";
   }

   IsoString Name() const override
   {
      return "Moon";
   }

   void ComputeState( vector_type& p, const TimePoint& t ) const override
   {
      Vector p_, pb_;
      m_embHandle.ComputeState( pb_, t );
      m_moonHandle.ComputeState( p_, t );
      p_ += pb_ - m_emb2Earth*p_;
      p = vector_type( p_ );
   }

   void ComputeState( vector_type& p, vector_type& v, const TimePoint& t ) const override
   {
      Vector p_, v_, pb_, vb_;
      m_embHandle.ComputeState( pb_, vb_, t );
      m_moonHandle.ComputeState( p_, v_, t );
      p_ += pb_ - m_emb2Earth*p_;
      v_ += vb_ - m_emb2Earth*v_;
      p = vector_type( p_ );
      v = vector_type( v_ );
   }

   vector_type Velocity( const TimePoint& t ) const override
   {
      Vector vb_ = m_embHandle.FirstDerivative( t );
      Vector v_ = m_moonHandle.FirstDerivative( t );
      return vector_type( v_ + (vb_ - m_emb2Earth*v_) );
   }

   vector_type Acceleration( const TimePoint& t ) const override
   {
      Vector ab_ = m_embHandle.SecondDerivative( t );
      Vector a_ = m_moonHandle.SecondDerivative( t );
      return vector_type( a_ + (ab_ - m_emb2Earth*a_) );
   }

   double TrueDistance( const TimePoint& t ) const override
   {
      return Position( t, "TDB" ).TrueDistance( m_moonHandle );
   }

   double LightTravelTime( const TimePoint& t ) const override
   {
      return Position( t, "TDB" ).LightTravelTime( m_moonHandle );
   }

   void GetRotation( scalar_type& a0, scalar_type& d0, scalar_type& W, const TimePoint& t ) const override
   {
      GetMoonRotation( a0, d0, W, t );
   }

private:

   mutable EphemerisFile::Handle m_embHandle;
   mutable EphemerisFile::Handle m_moonHandle;
           double                m_emb2Earth;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __Perturber_h

// ----------------------------------------------------------------------------
// EOF Perturber.h - Released 2024-06-18T15:49:25Z
