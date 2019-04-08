/*
 *  types.h
 *  waves2
 *
 *  Created by arun on 01/02/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef unsigned char	byte;
typedef int intx;

//for fixed
#define Q_FACTOR	12
#define FX_ONE		(1<<Q_FACTOR)
#define FF_ONE		4096.0f
#define ITOX(c)		((c)<<Q_FACTOR)
#define XTOI(c)		((c)>>Q_FACTOR)
#define FTOX(c)		((int)((c)*FF_ONE))
#define XTOF(c)		((float)((c)/FF_ONE))

#define MULTX64(arg1, arg2)	(((__int64_t)arg1*arg2)>>Q_FACTOR)
#define MULTX(arg1, arg2)	(int)(((__int64_t)arg1*arg2)>>Q_FACTOR)
#define DIVX(arg1, arg2)	(int)(ITOX((__int64_t)arg1)/arg2)

#define FX12TO16(arg1)		((arg1)*(1<<4))
#define FX16TOI(arg1)       ((arg1)>>16)
//#define FX16TO12(arg1)		((arg1)/(1<<4))
//#define FX12TO16(arg1)		((arg1)*(1<<16))//for fixed

#define GX_DELETE(x)		if(x){delete x; x=NULL;}
#define GX_DELETE_ARY(x)	if(x){delete [] x; x=NULL;}

#define GX_SWAP_INT(x1, x2)     { int t=x1; x1=x2; x2=t;    }
#define GX_SWAP_FLOAT(x1, x2)   { float t=x1; x1=x2; x2=t;    }

#define GX_MAX_INT  2147483647
#define GX_MIN_INT (-2147483647 - 1)

#if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)
	#define USE_OPENAL	1
#elif defined(ANDROID)
	#define USE_OPENSL	1
	//#define USE_OPENAL	1
#endif

#if GEAR_WINDOWS
#define GX_STRCPY(dst, src)    strcpy_s(dst, sizeof(dst), src)
#else
#define GX_STRCPY(dst, src)    strcpy(dst, src)
#endif

//#define GX_SWAP_INT(x1, x2)     { int t = x1; x1 = x2; x2 = t;    }
//#define GX_SWAP_FLOAT(x1, x2)   { float t = x1; x1 = x2; x2 = t;    }

extern "C" {
    void setResourcePath(const char* path);
    void setCommonResourcePath(const char* path);
    const char* getResourcePath(const char* relativepath);
    const char* getCommonResourcePath(const char* relativepath);
    const char* appendPath(const char* path1, const char* path2);
    
    int getRandom(int start, int end);  // including start, excluding end
}

#endif
