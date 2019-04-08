#ifndef PXMATH_H
#define PXMATH_H

#include "types.h"

//#include "gxDebug.h"

//#define EPSILON 0.01f

#ifndef M_PI      // may already been defined
#define M_PI  3.1415926535897932384626433832795f    // matches value in gcc v2 math.h
#endif

#define M_TWO_PI  6.283185307179586476925286766559f
#define M_180_PI  57.295779513082320876798154814105f
#define M_PI_180  0.017453292519943295769236907684886f

#define DEG2RAD(a) ((a)*M_PI_180)
#define RAD2DEG(a) ((a)*M_180_PI)

#define CLAMP(arg1, arg2, arg3) ((arg1<arg2)?arg2:(arg1>arg3)?arg3:arg1)

#ifndef MAX
#define  MAX( x, y ) ( ((x) > (y)) ? (x) : (y) )
#endif

#ifndef MIN
#define  MIN( x, y ) ( ((x) < (y)) ? (x) : (y) )
#endif

#ifndef ABS
#define ABS(val) (((val)>0)?(val):(-(val)))
#endif

//#define FTOS(X)				((PXshort)((X)*256.0f))
//#define STOF(X)				((PXfloat)(X)/2.0f)	

#ifdef __cplusplus
extern "C"
{
#endif
extern double sin(double);
extern double cos(double);
extern double tan(double);
extern double atan(double);
extern double sqrt(double);
#ifdef __cplusplus
}
#endif
/**/

const static unsigned short SinTable[] = {
    0, 71, 142, 214, 285, 356, 428, 499, 570, 640,
    711, 781, 851, 921, 990, 1060, 1129, 1197, 1265, 1333,
    1400, 1467, 1534, 1600, 1665, 1731, 1795, 1859, 1922, 1985,
    2048, 2109, 2170, 2230, 2290, 2349, 2407, 2465, 2521, 2577,
    2632, 2687, 2740, 2793, 2845, 2896, 2946, 2995, 3043, 3091,
    3137, 3183, 3227, 3271, 3313, 3355, 3395, 3435, 3473, 3510,
    3547, 3582, 3616, 3649, 3681, 3712, 3741, 3770, 3797, 3823,
    3848, 3872, 3895, 3917, 3937, 3956, 3974, 3991, 4006, 4020,
    4033, 4045, 4056, 4065, 4073, 4080, 4086, 4090, 4093, 4095,
    4095
};

static const unsigned short CosTable[] = {
    4096, 4095, 4093, 4090, 4086, 4080, 4073, 4065, 4056, 4045,
    4033, 4020, 4006, 3991, 3974, 3956, 3937, 3917, 3895, 3872,
    3848, 3823, 3797, 3770, 3741, 3712, 3681, 3649, 3616, 3582,
    3547, 3510, 3473, 3435, 3395, 3355, 3313, 3271, 3227, 3183,
    3137, 3091, 3043, 2995, 2946, 2896, 2845, 2793, 2740, 2687,
    2632, 2577, 2521, 2465, 2407, 2349, 2290, 2230, 2170, 2109,
    2047, 1985, 1922, 1859, 1795, 1731, 1665, 1600, 1534, 1467,
    1400, 1333, 1265, 1197, 1129, 1060, 990,  921,  851,  781,
    711,  640,  570,  499,  428,  356,  285,  214,  142,  71,
    0
};


class pxMath {
public:
    
//    static void printSinTable()
//    {
//        DEBUG_PRINT("size=%d", sizeof(SinTable)/2);
//        for(int x=0;x<=90; x++)
//        {
//            DEBUG_PRINT("costable %d,", FTOX(sin(DEG2RAD(x))));
//        }
//    }

	static float SINF(int ang) {
		return (float)sin(DEG2RAD(ang));
	}

	static float COSF(int ang) {
		return (float)cos(DEG2RAD(ang));
	}

	static float SINF(float ang) {
		return (float)sin(DEG2RAD(ang));
	}

	static float COSF(float ang) {
		return (float)cos(DEG2RAD(ang));
	}

	static int SINX(int i) {
		i = i%360;
		if(i<0)		i += 360;

		if(i>=0 && i<=90)			return(SinTable[i]);
		else if(i>90 && i<=180)		return(SinTable[180-i]);
		else if(i>180 && i<=270)	return(-SinTable[i-180]);
		else						return(-SinTable[360-i]);
	}

	static int COSX(int i) {
		i = i%360;
		if(i<0)		i += 360;

		if(i>=0 && i<=90)			return(CosTable[i]);
		else if(i>90 && i<=180)		return(-CosTable[180-i]);
		else if(i>180 && i<=270)	return(-CosTable[i-180]);
		else						return(CosTable[360-i]);
	}


	static float SQRT(float val) {
		return (float)sqrt(val);
	}
	
//    static unsigned int SQRT(unsigned int val)
//    {
//        return FTOX(SQRT(XTOF(val)));
//    }
//
//    static unsigned int SQRT(__int64_t val)
//    {
//        return FTOX(SQRT(XTOF(val)));
//    }
	
	static int RAND(int min, int max) {
		int r=rand();
		int val=min+r%(max-min);
		if(val<min || val>=max) {
			//NSLog(@"wrong");
		}
		return val;
	}
	
#define STEP(shift)                                                             \
    if(sqrtVal+(__int64_t)((unsigned int)0x40000000l>>shift) <= val) {          \
        val -= sqrtVal+(__int64_t)((unsigned int)0x40000000l>>shift);           \
        sqrtVal = (sqrtVal>>1) | (__int64_t)((unsigned int)0x40000000l>>shift); \
    } else {                                                                    \
        sqrtVal >>= 1;                                                          \
    }

    static __int64_t SQRT(__int64_t val) {
        // Note: This fast square root function only works with an even Q_FACTOR
        __int64_t sqrtVal = 0;

        STEP(0);
        STEP(2);
        STEP(4);
        STEP(6);
        STEP(8);
        STEP(10);
        STEP(12);
        STEP(14);
        STEP(16);
        STEP(18);
        STEP(20);
        STEP(22);
        STEP(24);
        STEP(26);
        STEP(28);
        STEP(30);

        if(sqrtVal < val)
        {
            ++sqrtVal;
        }

        sqrtVal <<= (Q_FACTOR>>1);
        return(sqrtVal);
    }
};


#endif //PXMATH_H
