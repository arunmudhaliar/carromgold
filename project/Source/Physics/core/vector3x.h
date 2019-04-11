/*
 *  vector3x.h
 *  waves2
 *
 *  Created by arun on 31/01/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef VECTOR3X_H
#define VECTOR3X_H

#include "pxMath.h"

class vector3x
{
public:
	int x, y, z;
	
	vector3x()								{	x=y=z=0;				}
	vector3x(int xx, int yy, int zz)		{	x=xx;	y=yy;	z=zz;	}
	vector3x(float xx, float yy, float zz)	{	x=FTOX(xx);	y=FTOX(yy);	z=FTOX(zz);	}
	vector3x(const vector3x& v)				{	x=v.x;	y=v.y;	z=v.z;	}
	
	~vector3x(){}
	
	vector3x operator+(const vector3x& v) const	{	return vector3x(x+v.x, y+v.y, z+v.z);	}
	vector3x operator-(const vector3x& v) const	{	return vector3x(x-v.x, y-v.y, z-v.z);	}
	
	vector3x operator*(const int n)			{	return vector3x(MULTX(x, n), MULTX(y, n), MULTX(z, n));	}
	vector3x operator*(const float n)		{	return vector3x(MULTX(x, FTOX(n)), MULTX(y, FTOX(n)), MULTX(z, FTOX(n)));	}
	
	
	void setx(int xx, int yy, int zz)		{	x=xx;	y=yy;	z=zz;	}
	
	void zerox()							{	x=y=z=0;	}
	
	__int64_t lengthSquaredx()
	{
		__int64_t a=MULTX64(x, x);
		__int64_t b=MULTX64(y, y);
		__int64_t c=MULTX64(z,z);
		
		return (a+b+c);
	}
	__int64_t lengthx()				{	return pxMath::SQRT(lengthSquaredx());				}
	__int64_t dotx(const vector3x& v)	{	return MULTX(x, v.x)+MULTX(y, v.y)+MULTX(z, v.z);	}
	
	vector3x crossx(const vector3x& v)
	{
		vector3x res;
		res.x=MULTX(y, v.z)-MULTX(z, v.y);
		res.y=MULTX(z, v.x)-MULTX(x, v.z);
		res.z=MULTX(x, v.y)-MULTX(y, v.x);
		return res;
	}
	
	__int64_t normalizex()
	{
		__int64_t n, oon;
		n=lengthSquaredx();
		if(n==0 || n==FX_ONE) return 0;
		
		n=pxMath::SQRT(n);
		oon = FX_ONE;
		
//        oon=DIVX(oon, n);
//
//        x=MULTX(x, oon);
//        y=MULTX(y, oon);
//        z=MULTX(z, oon);
		
        
        // Due to precision loss we multiply the numerator by 10 and divide after the calculations.
        // Note :- We did this because we know the square root values will be lesser. Don't use this for normal calculation involving big numbers.
        // It can overflow for big numbers.
        oon = ITOX((__int64_t)oon*10)/n;
        x=MULTX(x, oon);
        y=MULTX(y, oon);
        z=MULTX(z, oon);
        x/=10;
        y/=10;
        z/=10;
        oon/=10;
		return oon;
	}
	
};

#endif
