/*
 *  matrix4x4x.h
 *  waves2
 *
 *  Created by arun on 31/01/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef MATRIX4X4X_H
#define MATRIX4X4X_H

#include "pxMath.h"
#include "vector2x.h"
#include "vector3x.h"
#include <stdio.h>
#include <string.h>
#if GEAR_APPLE
    #include <OpenGL/gl.h>
#elif GEAR_ANDROID
    #include <GLES2/gl2.h>
#else
    #include <gl.h>
#endif

class matrix4x4x
{
protected:

public:
    int    m[16];
	matrix4x4x()
	{
		identityx();
	}
	
	matrix4x4x(int v)
	{
		for(int x=0;x<16;x++)
		{
			m[x]=v;
		}		
	}
	
	matrix4x4x(const matrix4x4x& matrix)
	{
		copyx(matrix);
	}
	
	virtual ~matrix4x4x()
	{}
	
	void operator=(const matrix4x4x& matrix)
	{
		copyx(matrix);
	}
	
	void copyx(const matrix4x4x& matrix)
	{
		for(int x=0;x<16;x++)
		{
			m[x]=matrix.m[x];
		}		
	}
	
	void identityx()
	{
		memset(m, 0, sizeof(m));
		m[0] = FX_ONE;
		m[5] = m[0];
		m[10] = m[0];
		m[15] = m[0];
	}
	
    void setScale(int sx, int sy, int sz)
    {
        m[0]=sx;
        m[5]=sy;
        m[10]=sz;
    }
    
    const int* getMatrix()  {   return m;   }
    
	matrix4x4x operator*(const matrix4x4x& matrix)
	{
		matrix4x4x dest;
		
		dest.m[0]	= MULTX(m[0], matrix.m[0])  + MULTX(m[4], matrix.m[1])  + MULTX(m[8] , matrix.m[2])  + MULTX(m[12], matrix.m[3]);
		dest.m[1]	= MULTX(m[1], matrix.m[0])  + MULTX(m[5], matrix.m[1])  + MULTX(m[9] , matrix.m[2])  + MULTX(m[13], matrix.m[3]);
		dest.m[2]	= MULTX(m[2], matrix.m[0])  + MULTX(m[6], matrix.m[1])  + MULTX(m[10], matrix.m[2])  + MULTX(m[14], matrix.m[3]);
		dest.m[3]	= MULTX(m[3], matrix.m[0])  + MULTX(m[7], matrix.m[1])  + MULTX(m[11], matrix.m[2])  + MULTX(m[15], matrix.m[3]);
		dest.m[4]	= MULTX(m[0], matrix.m[4])  + MULTX(m[4], matrix.m[5])  + MULTX(m[8] , matrix.m[6])  + MULTX(m[12], matrix.m[7]);
		dest.m[5]	= MULTX(m[1], matrix.m[4])  + MULTX(m[5], matrix.m[5])  + MULTX(m[9] , matrix.m[6])  + MULTX(m[13], matrix.m[7]);
		dest.m[6]	= MULTX(m[2], matrix.m[4])  + MULTX(m[6], matrix.m[5])  + MULTX(m[10], matrix.m[6])  + MULTX(m[14], matrix.m[7]);
		dest.m[7]	= MULTX(m[3], matrix.m[4])  + MULTX(m[7], matrix.m[5])  + MULTX(m[11], matrix.m[6])  + MULTX(m[15], matrix.m[7]);
		dest.m[8]	= MULTX(m[0], matrix.m[8])  + MULTX(m[4], matrix.m[9])  + MULTX(m[8] , matrix.m[10]) + MULTX(m[12], matrix.m[11]);
		dest.m[9]	= MULTX(m[1], matrix.m[8])  + MULTX(m[5], matrix.m[9])  + MULTX(m[9] , matrix.m[10]) + MULTX(m[13], matrix.m[11]);
		dest.m[10]	= MULTX(m[2], matrix.m[8])  + MULTX(m[6], matrix.m[9])  + MULTX(m[10], matrix.m[10]) + MULTX(m[14], matrix.m[11]);
		dest.m[11]	= MULTX(m[3], matrix.m[8])  + MULTX(m[7], matrix.m[9])  + MULTX(m[11], matrix.m[10]) + MULTX(m[15], matrix.m[11]);
		dest.m[12]	= MULTX(m[0], matrix.m[12]) + MULTX(m[4], matrix.m[13]) + MULTX(m[8] , matrix.m[14]) + MULTX(m[12], matrix.m[15]);
		dest.m[13]	= MULTX(m[1], matrix.m[12]) + MULTX(m[5], matrix.m[13]) + MULTX(m[9] , matrix.m[14]) + MULTX(m[13], matrix.m[15]);	
		dest.m[14]	= MULTX(m[2], matrix.m[12]) + MULTX(m[6], matrix.m[13]) + MULTX(m[10], matrix.m[14]) + MULTX(m[14], matrix.m[15]);
		dest.m[15]	= MULTX(m[3], matrix.m[12]) + MULTX(m[7], matrix.m[13]) + MULTX(m[11], matrix.m[14]) + MULTX(m[15], matrix.m[15]);

		return dest;
	}
	
	vector3x operator*(const vector3x& vec)
	{
		vector3x res;
		res.x = MULTX(m[0], vec.x) + MULTX(m[4], vec.y) + MULTX(m[8], vec.z)  + m[12];
		res.y = MULTX(m[1], vec.x) + MULTX(m[5], vec.y) + MULTX(m[9], vec.z)  + m[13];
		res.z = MULTX(m[2], vec.x) + MULTX(m[6], vec.y) + MULTX(m[10], vec.z) + m[14];
		return res;
	}

    vector2x operator*(const vector2x& vec)
	{
		vector2x res;
		res.x = MULTX(m[0], vec.x) + MULTX(m[4], vec.y) + m[12];
		res.y = MULTX(m[1], vec.x) + MULTX(m[5], vec.y) + m[13];
		return res;
	}
    
    vector2x operator%(const vector2x& vec)
	{
		vector2x res;
		res.x = MULTX(m[0], vec.x) + MULTX(m[4], vec.y);
		res.y = MULTX(m[1], vec.x) + MULTX(m[5], vec.y);
		return res;
	}
	
	void inversex()	{	*this=getInversex();	}

	matrix4x4x getInversex()
	{
		int *mat, *dst; 
		matrix4x4x res(0);
		dst = res.m;
		mat = m;
		
		/************************************************************
		 *
		 * input:
		 * mat - pointer to array of 16 no's (source matrix)
		 * output:
		 * dst - pointer to array of 16 no's (invert matrix)
		 *
		 *************************************************************/
		int i;
		int tmp[12];	/* temp array for pairs */
		int src[16];	/* array of transpose source matrix */
		int det;		/* determinant */
		
		/* transpose matrix */
		for( i = 0; i < 4; i++) 
		{
			src[i]   = mat[i*4];
			src[i+4] = mat[i*4 + 1];
			src[i+8] = mat[i*4 + 2];
			src[i+12]= mat[i*4 + 3];
		}

		/* calculate pairs for first 8 elements (cofactors) */
		tmp[0] = MULTX(src[10] , src[15]);
		tmp[1] = MULTX(src[11] , src[14]);
		tmp[2] = MULTX(src[9]  , src[15]);
		tmp[3] = MULTX(src[11] , src[13]);
		tmp[4] = MULTX(src[9]  , src[14]);
		tmp[5] = MULTX(src[10] , src[13]);
		tmp[6] = MULTX(src[8]  , src[15]);
		tmp[7] = MULTX(src[11] , src[12]);
		tmp[8] = MULTX(src[8]  , src[14]);
		tmp[9] = MULTX(src[10] , src[12]);
		tmp[10]= MULTX(src[8]  , src[13]);
		tmp[11]= MULTX(src[9]  , src[12]);
		
		/* calculate first 8 elements (cofactors) */
		dst[0]  = MULTX(tmp[0], src[5]) + MULTX(tmp[3], src[6]) + MULTX(tmp[4], src[7]);
		dst[0] -= MULTX(tmp[1], src[5]) + MULTX(tmp[2], src[6]) + MULTX(tmp[5], src[7]);
		dst[1]  = MULTX(tmp[1], src[4]) + MULTX(tmp[6], src[6]) + MULTX(tmp[9], src[7]);
		dst[1] -= MULTX(tmp[0], src[4]) + MULTX(tmp[7], src[6]) + MULTX(tmp[8], src[7]);
		dst[2]  = MULTX(tmp[2], src[4]) + MULTX(tmp[7], src[5]) + MULTX(tmp[10], src[7]);
		dst[2] -= MULTX(tmp[3], src[4]) + MULTX(tmp[6], src[5]) + MULTX(tmp[11], src[7]);
		dst[3]  = MULTX(tmp[5], src[4]) + MULTX(tmp[8], src[5]) + MULTX(tmp[11], src[6]);
		dst[3] -= MULTX(tmp[4], src[4]) + MULTX(tmp[9], src[5]) + MULTX(tmp[10], src[6]);
		dst[4]  = MULTX(tmp[1], src[1]) + MULTX(tmp[2], src[2]) + MULTX(tmp[5], src[3]);
		dst[4] -= MULTX(tmp[0], src[1]) + MULTX(tmp[3], src[2]) + MULTX(tmp[4], src[3]);
		dst[5]  = MULTX(tmp[0], src[0]) + MULTX(tmp[7], src[2]) + MULTX(tmp[8], src[3]);
		dst[5] -= MULTX(tmp[1], src[0]) + MULTX(tmp[6], src[2]) + MULTX(tmp[9], src[3]);
		dst[6]  = MULTX(tmp[3], src[0]) + MULTX(tmp[6], src[1]) + MULTX(tmp[11], src[3]);
		dst[6] -= MULTX(tmp[2], src[0]) + MULTX(tmp[7], src[1]) + MULTX(tmp[10], src[3]);
		dst[7]  = MULTX(tmp[4], src[0]) + MULTX(tmp[9], src[1]) + MULTX(tmp[10], src[2]);
		dst[7] -= MULTX(tmp[5], src[0]) + MULTX(tmp[8], src[1]) + MULTX(tmp[11], src[2]);
		
		/* calculate pairs for second 8 elements (cofactors) */
		tmp[0] = MULTX(src[2], src[7]);
		tmp[1] = MULTX(src[3], src[6]);
		tmp[2] = MULTX(src[1], src[7]);
		tmp[3] = MULTX(src[3], src[5]);
		tmp[4] = MULTX(src[1], src[6]);
		tmp[5] = MULTX(src[2], src[5]);
		tmp[6] = MULTX(src[0], src[7]);
		tmp[7] = MULTX(src[3], src[4]);
		tmp[8] = MULTX(src[0], src[6]);
		tmp[9] = MULTX(src[2], src[4]);
		tmp[10]= MULTX(src[0], src[5]);
		tmp[11]= MULTX(src[1], src[4]);
		
		/* calculate second 8 elements (cofactors) */
		dst[8]  = MULTX(tmp[0], src[13]) + MULTX(tmp[3], src[14]) + MULTX(tmp[4], src[15]);
		dst[8] -= MULTX(tmp[1], src[13]) + MULTX(tmp[2], src[14]) + MULTX(tmp[5], src[15]);
		dst[9]  = MULTX(tmp[1], src[12]) + MULTX(tmp[6], src[14]) + MULTX(tmp[9], src[15]);
		dst[9] -= MULTX(tmp[0], src[12]) + MULTX(tmp[7], src[14]) + MULTX(tmp[8], src[15]);
		dst[10] = MULTX(tmp[2], src[12]) + MULTX(tmp[7], src[13]) + MULTX(tmp[10], src[15]);
		dst[10]-= MULTX(tmp[3], src[12]) + MULTX(tmp[6], src[13]) + MULTX(tmp[11], src[15]);
		dst[11] = MULTX(tmp[5], src[12]) + MULTX(tmp[8], src[13]) + MULTX(tmp[11], src[14]);
		dst[11]-= MULTX(tmp[4], src[12]) + MULTX(tmp[9], src[13]) + MULTX(tmp[10], src[14]);
		dst[12] = MULTX(tmp[2], src[10]) + MULTX(tmp[5], src[11]) + MULTX(tmp[1], src[9]);
		dst[12]-= MULTX(tmp[4], src[11]) + MULTX(tmp[0], src[9])  + MULTX(tmp[3], src[10]);
		dst[13] = MULTX(tmp[8], src[11]) + MULTX(tmp[0], src[8])  + MULTX(tmp[7], src[10]);
		dst[13]-= MULTX(tmp[6], src[10]) + MULTX(tmp[9], src[11]) + MULTX(tmp[1], src[8]);
		dst[14] = MULTX(tmp[6], src[9])  + MULTX(tmp[11], src[11])+ MULTX(tmp[3], src[8]);
		dst[14]-= MULTX(tmp[10], src[11])+ MULTX(tmp[2], src[8])  + MULTX(tmp[7], src[9]);
		dst[15] = MULTX(tmp[10], src[10])+ MULTX(tmp[4], src[8])  + MULTX(tmp[9], src[9]);
		dst[15]-= MULTX(tmp[8], src[9])  + MULTX(tmp[11], src[10])+ MULTX(tmp[5], src[8]);
		
		/* calculate determinant */
		det = MULTX(src[0], dst[0]) + MULTX(src[1], dst[1]) + MULTX(src[2], dst[2]) + MULTX(src[3], dst[3]);
		
		if(det==0)				return NULL; //singular matrix
		
		/* calculate matrix inverse */
		det = DIVX(FX_ONE, det);
		
		for (i=0; i<16; i++)
			dst[i] = MULTX(dst[i], det);
		
		return res;
	}
	
	
	void setRotationMatrixx(int angle, bool X, bool Y, bool Z)
	{
		int sn,cs;
		sn = pxMath::SINX(angle);
		cs = pxMath::COSX(angle);
		
		identityx();
		
		int* _m=m;
		if(X)
		{
			_m[5]	= cs;
			_m[6]	= sn;
			_m[9]	= -sn;
			_m[10]	= cs;
		}
		
		if(Y)
		{
			_m[0]	= cs;
			_m[2]	= -sn;
			_m[8]	= sn;
			_m[10]	= cs;
		}
		
		if(Z)
		{
			_m[0]	= cs;
			_m[1]	= sn;
			_m[4]	= -sn;
			_m[5]	= cs;
		}
	}
	
	void setRotationMatrixx(int angle, const vector3x& axis)
	{
		int cs,sn;
		cs=pxMath::COSX(angle);
		sn=pxMath::SINX(angle);
		
		int x,y,z,one_cs;
		vector3x axisN(axis);
		axisN.normalizex();
		
		one_cs = FX_ONE - cs;
		x=axisN.x;
		y=axisN.y;
		z=axisN.z;
		
		int one_csx=MULTX(one_cs, x);
		int one_csy=MULTX(one_cs, y);
		int xx=MULTX(x, x);
		int yy=MULTX(y, y);
		int zz=MULTX(z, z);
		
		m[0]=FX_ONE+MULTX(one_cs, (xx-FX_ONE));
		m[1]=MULTX(z, sn) + MULTX(one_csx, y);
		m[2]=MULTX(-y, sn) + MULTX(one_csx, z);
		m[3]=0;
		
		m[4]=MULTX(-z, sn) + MULTX(one_csx, y);
		m[5]=FX_ONE+MULTX(one_cs, (yy-FX_ONE));
		m[6]=MULTX(x, sn) + MULTX(one_csy, z);
		m[7]=0;
		
		m[8]=MULTX(y, sn) + MULTX(one_csx, z);
		m[9]=MULTX(-x, sn) + MULTX(one_csy, z);
		m[10]=FX_ONE+MULTX(one_cs,(zz-FX_ONE));
		m[11]=0;
		
		m[12]=0;
		m[13]=0;
		m[14]=0;
		m[15]=FX_ONE;
	}
	
	void setRotationMatrixx(int angle, const int* axis)
	{
		setRotationMatrixx(angle, vector3x(axis[0], axis[1], axis[2]));
	}
	
	vector3x getXAxisx()		{	return vector3x(m[0], m[1], m[2]);		}
	vector3x getYAxisx()		{	return vector3x(m[4], m[5], m[6]);		}
	vector3x getZAxisx()		{	return vector3x(m[8], m[9], m[10]);		}
	vector3x getPositionx()		{	return vector3x(m[12], m[13], m[14]);	}
	vector2x getPosition2x()	{	return vector2x(m[12], m[13]);			}
	
	void setXAxisx(vector3x axis)
	{
		m[0]=axis.x; m[1]=axis.y; m[2]=axis.z;
	}

	void setYAxisx(vector3x axis)
	{
		m[4]=axis.x; m[5]=axis.y; m[6]=axis.z;
	}

	void setZAxisx(vector3x axis)
	{
		m[8]=axis.x; m[9]=axis.y; m[10]=axis.z;
	}
	
	void setPositionx(int x, int y, int z)	{	m[12]=x; m[13]=y; m[14]=z;	}
	
	void setDirx(vector3x* dir, vector3x* up=NULL)
	{
		vector3x aDir, aLeft, aUp;
		
		if(!dir)
		{	return;		}
		else
		{
			aDir = *dir;
			aDir.normalizex();
		}
		
		if(!up)
		{		
			aUp.x = 0;
			aUp.y = 0;
			aUp.z = 1;
		}
		else
		{
			aUp = *up;
			aUp.normalizex();
		}
		
		aLeft = aDir.crossx(aUp);
		aLeft.normalizex();
		
		if(up)
		{
			aUp	= aLeft.crossx(aDir);
			aUp.normalizex();
		}
		
		m[0]=aLeft.x; m[1]=aLeft.y;  m[2]=aLeft.z;
		m[4]=aDir.x; m[5]=aDir.y;  m[6]=aDir.z;
		m[8]=aUp.x; m[9]=aUp.y;  m[10]=aUp.z;
	}
    
    void drawGizmo()
    {
//        byte vertxlist[]={
//            0, 0,
//            5, 0,
//            0, 0,
//            0, 5
//        };
        
//        glPushMatrix();
//        glMultMatrixx(getMatrix());
//        glEnableClientState(GL_VERTEX_ARRAY);
//        glVertexPointer(2, GL_BYTE, 0, vertxlist);
//        glColor4f(1, 0, 0, 1);
//        glDrawArrays(GL_LINES, 0, 2);
//        glColor4f(0, 1, 0, 1);
//        glDrawArrays(GL_LINES, 2, 2);
//        glDisableClientState(GL_VERTEX_ARRAY);
//        glPopMatrix();
    }
};

#endif
