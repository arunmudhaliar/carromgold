/*
 *  vector2.h
 *  waves2
 *
 *  Created by arun on 18/02/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef VECTOR2_H
#define VECTOR2_H

#include "pxMath.h"

template<class T>
class vector2;
typedef vector2<float> vector2f;

template<class T>
class vector2;
typedef vector2<int> vector2i;

template<class T>
class vector2
{
public:
	T x, y;
	
	vector2<T>()							{	x=0;	y=0;	}
	vector2<T>(const T ax, const T ay)		{	x=ax;	y=ay;	}
	vector2<T>(const vector2<T>& v)			{	x=v.x;	y=v.y;	}
	
	void operator=(const vector2<T>& v)		{	x=v.x;	y=v.y;	}
	
	vector2<T> operator-()					{	return vector2<T>(-x, -y);	}
	
	vector2<T> operator+(const vector2<T>& v)	{	return vector2<T>(x+v.x, y+v.y);	}
	vector2<T> operator-(const vector2<T>& v)	{	return vector2<T>(x-v.x, y-v.y);	}
	
    void operator+=(const vector2<T>& v) 	{	x+=v.x; y+=v.y;	}
	void operator-=(const vector2<T>& v)	{	x-=v.x; y-=v.y;	}

	vector2<T> operator*(const int n)		{	return vector2<T>(x*n, y*n);	}
	vector2<T> operator*(const float n)			{	return vector2<T>(x*n, y*n);	}
	
	vector2<T> operator/(const int n)			{	return vector2<T>(x/n, y/n);	}
	vector2<T> operator/(const float n)			{	return vector2<T>(x/n, y/n);	}

	void operator/=(const int n)			{	x/=n, y/=n;	}
	void operator/=(const float n)			{	x/=n, y/=n;	}

	
	void set(T xx, T yy)					{	x=xx;	y=yy;	}
	
	void zero()								{	x=y=0;	}
	
	T LengthSquare() const					{	return((x*x)+(y*y));					}
	T Length() const						{	return pxMath::SQRT((float)LengthSquare());	}
	T Dot(const vector2<T>& v) const		{	return ((x*v.x)+(y*v.y));		}
	
	
	T Normalize()
	{
		T n, oon;
		n = (x*x)+(y*y);
		if (n==0 || n==1)
			return 0;
		n = pxMath::SQRT(n);
		oon = 1;
		oon /= n;
		x *= oon;
		y *= oon;
		
		return oon;
	}
};

#endif
