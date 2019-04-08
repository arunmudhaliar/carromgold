#pragma once

//#include "gxMath.h"
//#include "basicIncludes.h"

template<class T> class vector4;
typedef vector4<float> vector4f;

template<class T> class vector4;
typedef vector4<int> vector4i;

template<class T>
class vector4
{
public:
	T x, y, z, w;
	
	vector4<T>()								{	x=y=z=w=0;						}
	vector4<T>(int xx, int yy, int zz, int ww)	{	x=xx;	y=yy;	z=zz;	w=ww;	}
	vector4<T>(float xx, float yy, float zz, float ww)	{	x=xx;	y=yy;	z=zz;	w=ww;	}
	vector4<T>(const vector4<T>& v)				{	x=v.x;	y=v.y;	z=v.z;	w=v.w;	}
	vector4<T>(T *v)							{	x=v[0];	y=v[1];	z=v[2];	w=v[3];	}

	~vector4<T>(){}
	
	vector4<T> operator+( vector4<T>& v)	{	return vector4<T>(x+v.x, y+v.y, z+v.z, w+v.w);	}
	vector4<T> operator-( vector4<T>& v)	{	return vector4<T>(x-v.x, y-v.y, z-v.z, w-v.w);	}
	
	vector4<T> operator*(const int n)			{	return vector4<T>(x*n, y*n, z*n, w*n);	}
	vector4<T> operator*(const float n)			{	return vector4<T>(x*n, y*n, z*n, w*n);	}
	
	
	vector4<T> operator/(const short n)	const			{	return vector4<T>(x/n, y/n, z/n, w/n);			}
	vector4<T> operator/(const unsigned short n)const	{	return vector4<T>(x/n, y/n, z/n, w/n);			}
	vector4<T> operator/(const int n)		const		{	return vector4<T>(x/n, y/n, z/n, w/n);			}
	vector4<T> operator/(const unsigned int n)	const	{	return vector4<T>(x/n, y/n, z/n, w/n);			}
	vector4<T> operator/(const float n)	const			{	return vector4<T>(x/n, y/n, z/n, w/n);			}

	void set(T xx, T yy, T zz, T ww)		{	x=xx;	y=yy;	z=zz;	w=ww;	}
	
	void zero()								{	x=y=z=w=0;	}
	
	T lengthSquared()
	{		
		return (x*x+y*y+z*z+w*w);
	}

	T length()					{	return gxMath::SQRT(lengthSquared());				}
	T dot(const vector4<T>& v)	{	return x*v.x + y*v.y + z*v.z + w*v.w;	}
	
	//vector4<T> cross(const vector4<T>& v)
	//{
	//	vector4<T> res;
	//	res.x=y*v.z-z*v.y;
	//	res.y=z*v.x-x*v.z;
	//	res.z=x*v.y-y*v.x;
	//	return res;
	//}
	
	T normalize()
	{
		T n, oon;
		n=lengthSquared();
		if(n==0.0f || n==1.0f) return 0.0f;
		
		n=gxMath::SQRT(n);
		oon = 1.0f;
		
		oon/=n;
		
		x*=oon;
		y*=oon;
		z*=oon;
		w*=oon;

		return oon;
	}
	
};
