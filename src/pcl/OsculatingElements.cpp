//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.11
// ----------------------------------------------------------------------------
// pcl/OsculatingElements.cpp - Released 2024-05-07T15:27:40Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/Matrix.h>
#include <pcl/OsculatingElements.h>

#define el_a   this->a
#define el_q   this->q
#define el_e   this->e
#define el_M   this->M
#define el_T   this->T
#define el_i   this->i
#define el_O   this->O
#define el_w   this->w

namespace pcl
{

// ----------------------------------------------------------------------------

// Default mass parameter for Sun (au^3/day^2)
// Value taken from DE440:
// GMS = k^2 = 2.9591220828411956e-04
static const double GMS_de440 = 2.9591220828411956e-04;

// ----------------------------------------------------------------------------

OsculatingElements::OsculatingElements( const Vector& r, const Vector& v, const TimePoint& t, double GM, double m )
{
   // Gravitational constant
   if ( GM <= 0 )
      GM = GMS_de440;

   // Combined mass in gravitational units.
   double mu = (1 + m) * GM;

   double rm = r.L2Norm();
   double v2 = v.Dot( v );
   double rv = r.Dot( v );

   // Angular momentum vector
   Vector h = r.Cross( v ); // au^2 * day^-1
   double p = h.Dot( h )/mu;

   // Eccentricity vector
   Vector e = (v2/mu - 1/rm)*r - (rv/mu)*v;
   // alternative formulation:
   // Vector e = v.Cross( h )/mu - r/rm;

   // Ascending node vector
   Vector N( -h[1], h[0], 0.0 ); // = Vector( 0, 0, 1 ).Cross( h );
   double n = N.L2Norm();

   // Eccentricity
   el_e = e.L2Norm();
   bool parabolic = IsNearParabolic();
   bool circular = Abs( el_e ) < 1.0e-10;

   // Semimajor axis and perifocal distance
   if ( parabolic )
   {
      el_q = p/(1 + el_e);
      el_a = SemimajorAxisFromPerihelionDistance();
   }
   else
   {
      el_a = 1/(2/rm - v2/mu);
      el_q = PerihelionDistanceFromSemimajorAxis();
   }

   // Inclination
   el_i = ArcCos( h[2]/h.L2Norm() );

   // Longitude of the ascending node
   el_O = Norm2Pi( ArcTan( h[0], -h[1] ) );

   // Argument of the perifocus
   if ( circular )
      el_w = 0;
   else
      el_w = Norm2Pi( ArcCos( N.Dot( e )/n/el_e ) );

   // Mean anomaly
   if ( parabolic )
   {
      // Parabolic and near-parabolic
      double D = rv/Sqrt( mu );
      el_M = el_q*D + D*D*D/6;
      // Time of perifocal passage
      el_T = t - el_M/Sqrt( mu );
   }
   else
   {
      if ( circular )
         el_M = 0;
      else
      {
         double yb = rv/el_e * Sqrt( p/mu );
         if ( el_e < 1 )
         {
            // Elliptic
            double xb = (p - rm)/el_e;
            double cE = xb/el_a + el_e;
            double sE = yb/el_a/Sqrt( 1 - el_e*el_e );
            double E = ArcTan( sE, cE );
            el_M = E - el_e*sE;
         }
         else
         {
            // Hyperbolic
            double sH = -yb/el_a/Sqrt( el_e*el_e - 1 );
            double H = ArcSinh( sH );
            el_M = el_e*sH - H;
         }
      }

      // Time of perifocal passage
      el_T = t - el_M/MeanMotion( GM, m );
   }
   el_M = Norm2Pi( el_M );
}

// ----------------------------------------------------------------------------

void OsculatingElements::ToStateVectors( Vector& r, Vector& v, const TimePoint& t, double GM, double m ) const
{
   double xp, yp, xv, yv;
   ToPerifocalPositionAndVelocity( xp, yp, xv, yv, t, GM, m );

   double si, ci;
   SinCos( el_i, si, ci );
   double sO, cO;
   SinCos( el_O, sO, cO );
   double sw, cw;
   SinCos( el_w, sw, cw );

   // Unit vector P
   double px =  cw*cO - sw*sO*ci;
   double py =  cw*sO + sw*cO*ci;
   double pz =  sw*si;

   // Unit vector Q
   double qx = -sw*cO - cw*sO*ci;
   double qy = -sw*sO + cw*cO*ci;
   double qz =  cw*si;

   /*
    * State vectors referred to the fundamental plane.
    */
   r = Vector( xp*px + yp*qx // position vector
             , xp*py + yp*qy
             , xp*pz + yp*qz );
   v = Vector( xv*px + yv*qx // velocity vector
             , xv*py + yv*qy
             , xv*pz + yv*qz );
}

// ----------------------------------------------------------------------------

double OsculatingElements::MeanMotion( double GM, double m ) const
{
   if ( GM <= 0 )
      GM = GMS_de440;

   if ( IsNearParabolic() )
   {
      double ai = Abs( 1 - el_e )/el_q;
      if ( ai > 1.0e-5 )
         return Sqrt( GM*(1 + m)*ai*ai*ai );
      return Sqrt( GM*(1 + m) );
   }

   double a = Abs( el_a );
   return Sqrt( GM*(1 + m)/a/a/a );
}

// ----------------------------------------------------------------------------

double OsculatingElements::OrbitalPeriod( double GM, double m ) const
{
   if ( this->e < 1 )
   {
      if ( GM <= 0 )
         GM = GMS_de440;
      double a = Abs( el_a );
      return TwoPi() * Sqrt( a*a*a/GM/(1 + m) );
   }

   return 0;
}

// ----------------------------------------------------------------------------

void OsculatingElements::ToPerifocalPositionAndVelocity( double& xp, double& yp, double& xv, double& yv, const TimePoint& t, double GM, double m ) const
{
   // Maximum number of iterations for calculation of eccentric anomalies.
   const int maxIt = 50;

   // Gravitational constant.
   if ( GM <= 0 )
      GM = GMS_de440;

   // Combined mass in gravitational units.
   double mu = GM * (1 + m);

   // Accuracy for iterative calculation of the eccentric anomaly. When the
   // routine does not converge after a reasonable number of iterations, we'll
   // increase this limit adaptively.
   double eps = 2*std::numeric_limits<double>::epsilon();

   if ( IsNearParabolic() )
   {
      /*
       * For parabolic and near-parabolic orbits, use a numerically stable
       * formulation based on Stumpff functions.
       *
       * Based on the implementation described in the book:
       * Oliver Montenbruck, Thomas Pfleger (2003), Astronomy on the Personal
       * Computer, Fourth Edition (Springer), Section 4.4.
       */
      double E2 = 0;
      double f = el_e/2;
      double c1, c2, c3;
      double u, u2;
      double dt = Sqrt( GM )*(t - el_T);
      for ( int i = 0, n = 0;; )
      {
         double E20 = E2;
         double A = 3*Sqrt( f/el_q/el_q/el_q )/2 * dt;
         double B = Pow( A + Sqrt( 1 + A*A ), 1.0/3 );
         u = B - 1/B;
         u2 = u*u;
         E2 = u2*(1 - el_e)/f;
         c1 = c2 = c3 = 0;
         double t = 1;
         for ( int n = 1;; ++n )
         {
            c1 += t; t /= 2*n;
            c2 += t; t /= 2*n + 1;
            c3 += t; t *= -E2;
            if ( Abs( t ) < 2*std::numeric_limits<double>::epsilon() )
               break;
         }
         f = 3*el_e*c3;

         if ( Abs( E2 - E20 ) < eps )
            break;
         if ( ++i == maxIt )
         {
            if ( ++n == 3 )
               throw Error( String().Format( "OsculatingElements: Finding near-parabolic eccentric anomaly (e=%.6f q=%.6f): No convergence.", el_e, el_q ) );
            eps *= 10;
            i = 0;
         }
      }

      double rv = el_q * (1 + u2*c2*el_e/f);
      double kv = Sqrt( mu/el_q/(1 + el_e) );

      xp =  el_q * (1 - u2*c2/f);
      yp =  el_q * Sqrt( (1 + el_e)/f )*u*c1;
      xv = -kv * yp/rv;
      yv =  kv * (xp/rv + el_e);
   }
   else if ( el_e < 1 )
   {
      /*
       * Elliptic motion
       */
      // Find eccentric anomaly by the Newton-Raphson method.
      double E = (el_e < 0.8) ? el_M : Pi(), sE, cE;
      for ( int i = 0, n = 0;; )
      {
         SinCos( E, sE, cE );
         double f = E - el_e*sE - el_M;
         if ( Abs( f ) < eps )
            break;
         if ( ++i == maxIt )
         {
            if ( ++n == 3 )
               throw Error( String().Format( "OsculatingElements: Finding eccentric anomaly (e=%.6f M=%.6f): No convergence.", el_e, Deg( el_M ) ) );
            eps *= 10;
            i = 0;
         }
         double dfdE = 1 - el_e*cE;
         E -= f/dfdE;
      }

      double a = el_a;
      if ( a == 0 )
         a = SemimajorAxisFromPerihelionDistance();
      if ( 1 + a == 1 )
         throw Error( "OsculatingElements: Invalid or undefined semimajor axis / perihelion distance." );
      // The semimajor axis should be > 0 for e < 1 - be resilient to input errors.
      if ( a < 0 ) // ?!
         a = -a;

      double r = a*(1 - el_e*cE);
      double Edot = Sqrt( mu/a )/r;
      double b = a*Sqrt( 1 - el_e*el_e );

      xp =  a*(cE - el_e);
      yp =  b*sE;
      xv = -a*Edot*sE;
      yv =  b*Edot*cE;
   }
   else // el_e > 1
   {
      /*
       * Hyperbolic motion
       */
      double a = el_a;
      if ( a == 0 )
         a = SemimajorAxisFromPerihelionDistance();
      if ( 1 + a == 1 )
         throw Error( "OsculatingElements: Invalid or undefined semimajor axis / perihelion distance." );
      // The semimajor axis should be < 0 for e > 1 - be resilient to input errors.
      if ( a > 0 ) // ?!
         a = -a;

      // Find hyperbolic eccentric anomaly by the Newton-Raphson method.
      double H = el_M, sH, cH;
      for ( int i = 0, n = 0;; )
      {
         sH = Sinh( H );
         cH = Cosh( H );
         double f = el_e*sH - H - el_M;
         if ( Abs( f ) < eps )
            break;
         if ( ++i == maxIt )
         {
            if ( ++n == 3 )
               throw Error( String().Format( "OsculatingElements: Finding hyperbolic eccentric anomaly (e=%.6f M=%.6f): No convergence.", el_e, Deg( el_M ) ) );
            eps *= 10;
            i = 0;
         }
         double dfdH = el_e*cH - 1;
         H -= f/dfdH;
      }

      double r = a*(1 - el_e*cH);
      double Hdot = Sqrt( -mu/a )/r;
      double b = -a*Sqrt( el_e*el_e - 1 );

      xp =  a*(cH - el_e);
      yp =  b*sH;
      xv =  a*Hdot*sH;
      yv =  b*Hdot*cH;
   }
}

// ----------------------------------------------------------------------------

void OsculatingElements::GetOrbitOrientationFromStateVectors( double& i, double& O, double& w, const Vector& r, const Vector& v, double GM, double m )
{
   // Gravitational constant
   if ( GM <= 0 )
      GM = GMS_de440;

   // Combined mass in gravitational units.
   double mu = (1 + m) * GM;

   // Angular momentum vector
   Vector h = r.Cross( v ); // au^2 * day^-1

   // Eccentricity vector
   Vector e = (v.Dot( v )/mu - 1/r.L2Norm())*r - (r.Dot( v )/mu)*v;

   // Ascending node vector
   Vector N( -h[1], h[0], 0.0 ); // = Vector( 0, 0, 1 ).Cross( h );

   // Inclination
   i = ArcCos( h[2]/h.L2Norm() );

   // Longitude of the ascending node
   O = Norm2Pi( ArcTan( h[0], -h[1] ) );

   // Argument of the perifocus
   w = Norm2Pi( ArcCos( N.Dot( e )/N.L2Norm()/e.L2Norm() ) );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/OsculatingElements.cpp - Released 2024-05-07T15:27:40Z
