/*
 *  transformx.h
 *  waves2
 *
 *  Created by arun on 31/01/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef TRANSFORMX_H
#define TRANSFORMX_H

#include "matrix4x4x.h"

class transformx : public matrix4x4x
{
public:
	
	transformx():matrix4x4x()
	{
		transformationChangedx();
	}
	
	virtual ~transformx(){}
	
	int* getOpenGLMatrix()	{	return m;	}
	
	void updatePositionx(int aXdt, int aYdt, int aZdt)
	{
		m[12]	+= aXdt;
		m[13]	+= aYdt;
		m[14]	+= aZdt;
		
		transformationChangedx();
	}
	
	void updateLocalPositionx(int aXdt, int aYdt, int aZdt)
	{
		if(aXdt!=0)	translateLocalXx(aXdt);
		if(aYdt!=0)	translateLocalYx(aYdt);
		if(aZdt!=0)	translateLocalZx(aZdt);
		
		transformationChangedx();
	}
	
	void translateLocalXx(int aXdt)
	{
		m[12] += MULTX(aXdt, m[0]);
		m[13] += MULTX(aXdt, m[1]);
		m[14] += MULTX(aXdt, m[2]);
	}
	void translateLocalYx(int aYdt)
	{
		m[12] += MULTX(aYdt, m[4]);
		m[13] += MULTX(aYdt, m[5]);
		m[14] += MULTX(aYdt, m[6]);
	}
	void translateLocalZx(int aZdt)
	{
		m[12] += MULTX(aZdt, m[8]);
		m[13] += MULTX(aZdt, m[9]);
		m[14] += MULTX(aZdt, m[10]);
	}
	
    void scalex(int scale)
    {
        matrix4x4x scaleTM;

        scaleTM.setScale(scale, scale, scale);
        //multiply with transform matrix
		matrix4x4x* p=this;
		*p = (scaleTM * (*this));
		
		transformationChangedx();    
    }
	
	void rotateWorldXx(int aAngleInDeg)
	{
		matrix4x4x aRotMat;
		
		aRotMat.setRotationMatrixx(aAngleInDeg, true, false, false);
		//multiply with transform matrix
		matrix4x4x* p=this;
		*p = (aRotMat * (*this));
		
		transformationChangedx();
	}
	
	void rotateWorldYx(int aAngleInDeg)
	{
		matrix4x4x aRotMat;
		
		aRotMat.setRotationMatrixx(aAngleInDeg, false, true, false);
		//multiply with transform matrix
		matrix4x4x* p=this;
		*p = (aRotMat * (*this));
		
		transformationChangedx();
	}
	
	void rotateWorldZx(int aAngleInDeg)
	{
		matrix4x4x aRotMat;
		
		aRotMat.setRotationMatrixx(aAngleInDeg, false, false, true);
		//multiply with transform matrix
		matrix4x4x* p=this;
		*p = (aRotMat * (*this));
		
		transformationChangedx();
	}
	
	void rotateLocalXx(int aAngleInDeg)
	{
		matrix4x4x	aRotMat;
		
		aRotMat.setRotationMatrixx(aAngleInDeg, getXAxisx());
		vector3x	aTransPoint;
		aTransPoint.x=m[12];
		aTransPoint.y=m[13];
		aTransPoint.z=m[14];
		
		aRotMat.setPositionx(-aTransPoint.x, -aTransPoint.y, -aTransPoint.z);
		
		//multiply with transform matrix
		matrix4x4x* p=this;
		*p = (aRotMat * (*this));
		
		m[12]=aTransPoint.x;
		m[13]=aTransPoint.y;
		m[14]=aTransPoint.z;
		
		transformationChangedx();
	}
	
	void rotateLocalYx(int aAngleInDeg)
	{
		matrix4x4x	aRotMat;
		
		aRotMat.setRotationMatrixx(aAngleInDeg, getYAxisx());
		vector3x	aTransPoint;
		aTransPoint.x=m[12];
		aTransPoint.y=m[13];
		aTransPoint.z=m[14];
		
		aRotMat.setPositionx(-aTransPoint.x, -aTransPoint.y, -aTransPoint.z);
		
		//multiply with transform matrix
		matrix4x4x* p=this;
		*p = (aRotMat * (*this));
		
		m[12]=aTransPoint.x;
		m[13]=aTransPoint.y;
		m[14]=aTransPoint.z;
		
		transformationChangedx();
	}
	
	void rotateLocalZx(int aAngleInDeg)
	{
		matrix4x4x	aRotMat;
		
		aRotMat.setRotationMatrixx(aAngleInDeg, getZAxisx());
		vector3x	aTransPoint;
		aTransPoint.x=m[12];
		aTransPoint.y=m[13];
		aTransPoint.z=m[14];
		
		aRotMat.setPositionx(-aTransPoint.x, -aTransPoint.y, -aTransPoint.z);
		
		//multiply with transform matrix
		matrix4x4x* p=this;
		*p = (aRotMat * (*this));
		
		m[12]=aTransPoint.x;
		m[13]=aTransPoint.y;
		m[14]=aTransPoint.z;
		
		transformationChangedx();
	}
	
	void rotateArb(int aAngleInDeg, int* aAxis, vector3x aPos)
	{
		matrix4x4x	aRotMat;
		aRotMat.setRotationMatrixx(aAngleInDeg, &aAxis[0]);
		
		vector3x	aTransPoint;
		m[12]-=aPos.x;
		m[13]-=aPos.y;
		m[14]-=aPos.z;
		
		//multiply with transform matrix
		matrix4x4x* p=this;
		*p = (aRotMat * (*this));
		
		m[12]+=aPos.x;
		m[13]+=aPos.y;
		m[14]+=aPos.z;
		
		transformationChangedx();
	}
	
	void setDirectionx(vector3x* aDir, vector3x* aUp=NULL)
	{
		setDirx(aDir, aUp);
		transformationChangedx();
	}
	
	void setPositionx(int x, int y, int z)
	{
		m[12]=x;
		m[13]=y;
		m[14]=z;
		transformationChangedx();
	}
    
    void setPositionx(const vector2x& pos)
    {
        m[12]=pos.x;
        m[13]=pos.y;
        transformationChangedx();
    }
	
	virtual void transformationChangedx(){}
};

#endif
