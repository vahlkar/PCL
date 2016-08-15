//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.14.0193
// ----------------------------------------------------------------------------
// Alignment.h - Released 2016/06/17 12:50:37 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2016 Klaus Kretzschmar
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
//    by Pleiades Astrophoto and its contributors (http://pixinsight.com/)."
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

#ifndef __Alignment_h
#define __Alignment_h

#include <pcl/Matrix.h>
#include <pcl/Vector.h>
#include <pcl/Array.h>
#include <pcl/MetaParameter.h>

namespace pcl
{

enum PierSide {
	West,
	East
};

struct SyncDataPoint {
	double    localSiderialTime;
	double    celestialRA; // true position of the telescope on the sky
	double    celestialDEC;
	double    telecopeRA;  //
	double    telecopeDEC;
	pcl_enum  pierSide;
	bool      enabled;

	bool operator ==( const SyncDataPoint& rhs ) const
	{
		return     localSiderialTime == rhs.localSiderialTime
		       &&  celestialRA       == rhs.celestialRA
			   &&  celestialDEC      == rhs.celestialDEC
			   &&  telecopeRA        == rhs.telecopeRA
			   &&  pierSide          == rhs.pierSide;
	 }
};

/*
 * Alignment interface
 */
class AlignmentModel {

public:

	AlignmentModel()
	{
	}

	virtual ~AlignmentModel()
	{
	}

	virtual void Apply(double& hourAngleCor, double& decCor, const double hourAngle, const double dec) = 0;

	virtual void fitModel(const Array<SyncDataPoint>& syncPointArray, pcl_enum pierSide) = 0;

	virtual void writeObject(const String& fileName, pcl_enum pierSide) = 0;

	virtual void readObject(const String& fileName) = 0;

	static void getPseudoInverse(Matrix& pseudoInverse, const Matrix& matrix);

};

// ----------------------------------------------------------------------------


/*
 * Analytical telescope pointing model.
 *
 * Model to capture the effects of
 * - zero point offsets in declination and hour angle
 * - ...
 *
 * http://www.boulder.swri.edu/~buie/idl/downloads/pointing/pointing.pdf
 *
 *
 */
 class LowellPointingModel : public AlignmentModel {
 public:

	LowellPointingModel() : AlignmentModel (), m_modelCoefficientsWest(4), m_modelCoefficientsEast(4), m_modelHACoefficientsWest(4), m_modelHACoefficientsEast(4)
 	{
 	}
	virtual ~LowellPointingModel()
	{

	}

	virtual void Apply(double& hourAngleCor, double& decCor, const double hourAngle, const double dec) ;

	virtual void fitModel(const Array<SyncDataPoint>& syncPointArray, pcl_enum pierSide);

	static AlignmentModel* create(){
		return new LowellPointingModel();
	}

	virtual void writeObject(const String& fileName, pcl_enum pierSide);

	virtual void readObject(const String& fileName);

private:

	Vector m_modelCoefficientsWest;
	Vector m_modelCoefficientsEast;
	Vector m_modelHACoefficientsWest;
	Vector m_modelHACoefficientsEast;
};



 /*
  * Analytical telescope pointing model.
  *
  * Model to capture the effects of
  * - zero point offsets in declination and hour angle
  * - ...
  *
  * http://www.boulder.swri.edu/~buie/idl/downloads/pointing/pointing.pdf
  *
  *
  */
  class TpointPointingModel : public AlignmentModel {
  public:

	 TpointPointingModel(double siteLatitude) : AlignmentModel (), m_numOfModelParameters(9), m_siteLatitude(siteLatitude * Const<double>::rad()), m_pointingModelWest(nullptr), m_pointingModelEast(nullptr),m_modelConfig(((uint32_t)1 << m_numOfModelParameters) - 1)
	 {
		 m_pointingModelWest = new Vector(m_numOfModelParameters);
		 m_pointingModelEast = new Vector(m_numOfModelParameters);
	 }
	TpointPointingModel(double siteLatitude,uint32_t modelConfig) : AlignmentModel (), m_numOfModelParameters(9), m_siteLatitude(siteLatitude * Const<double>::rad()), m_pointingModelWest(nullptr), m_pointingModelEast(nullptr),m_modelConfig(modelConfig)
  	{
		m_pointingModelWest = new Vector(m_numOfModelParameters);
		m_pointingModelEast = new Vector(m_numOfModelParameters);
  	}

 	virtual ~TpointPointingModel()
 	{
 		delete m_pointingModelWest;
 		delete m_pointingModelEast;
 	}

 	virtual void Apply(double& hourAngleCor, double& decCor, const double hourAngle, const double dec) ;

 	virtual void fitModel(const Array<SyncDataPoint>& syncPointArray, pcl_enum pierSide);

 	// siteLatidude given in degrees
 	static AlignmentModel* create( double siteLatitude, uint32_t modelConfig){
 		return new TpointPointingModel(siteLatitude,modelConfig);
 	}
 	static AlignmentModel* create( double siteLatitude){
 		return new TpointPointingModel(siteLatitude);
 	}

 	virtual void writeObject(const String& fileName, pcl_enum pierSide);

 	virtual void readObject(const String& fileName);

 private:

 	void evaluateBasis(Matrix& basisMatrix, double hourAngle, double dec);

 	size_t m_numOfModelParameters;
 	double m_siteLatitude; // in radians

 	Vector*  m_pointingModelWest;
 	Vector*  m_pointingModelEast;
 	uint32_t m_modelConfig;
 };

 /*
  * Analytical telescope pointing model,
  * using Spherical Harmonics to model
  * anisotropic effects.
  *
  *
  * http://
  *
  *
  */
 class AnalyticalPointingModel : public AlignmentModel {
 public:
	 AnalyticalPointingModel(size_t spericalHarmonicsOrder):AlignmentModel (), m_orderOfSphericalHarmonics(spericalHarmonicsOrder),m_numOfModelParameters(0),m_modelParameters(nullptr){
		 if (m_orderOfSphericalHarmonics == 0){
			 m_numOfModelParameters=6;
		 } else if (m_orderOfSphericalHarmonics == 1){
			 m_numOfModelParameters=22;
		 } else if (m_orderOfSphericalHarmonics == 2){
			 m_numOfModelParameters=46;
		 } else {
			 throw Error( "Internal error: AnalyticalPointingModel: Order of spherical harmonics > 2 not supported" );
		 }

		 m_modelParameters = new Vector(m_numOfModelParameters);
	 }

	 virtual ~AnalyticalPointingModel(){
		 delete m_modelParameters;
	 }

	 virtual void Apply(double& hourAngleCor, double& decCor,  double hourAngle, double dec);

	 virtual void fitModel(const Array<SyncDataPoint>& syncPointArray, pcl_enum pierSide);

	 static AlignmentModel* create(size_t spericalHarmonicsOrder){
		 return new AnalyticalPointingModel(spericalHarmonicsOrder);
	 }

	 virtual void writeObject(const String& fileName, pcl_enum pierSide);

	 virtual void readObject(const String& fileName);
 private:

	 static void evaluateBasis(Matrix& basisMatrix, double hourAngle, double dec);

	 size_t m_orderOfSphericalHarmonics;
	 size_t m_numOfModelParameters;

	 Vector* m_modelParameters;

 };


} // pcl

#endif   // __Alignment_h

// ----------------------------------------------------------------------------
// EOF Alignment.h - Released 2016/06/17 12:50:37 UTC
