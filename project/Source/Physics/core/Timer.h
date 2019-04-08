#pragma once

#include "types.h"

class Timer
{
public:
	
	static void init();				//should be called when game initializes
	static void update();			//update the timer variables for each frame
	static void update(float targetFPS);
	static void reset();			//resets the timer variables
	
	static float getDtinSec()		{	return timerDTInSec;			}
	static int getDTinMilliSec()	{	return timerDTInMilliSec;		}
	static float getFPS()			{	return timerFPS;                }
	static float getElapsedTime()	{	return timerElapsedTimeInSec;	}
	
	static double getCurrentTimeInSec();
	static unsigned long getCurrentTimeInMilliSec();
	
	static void setTimeScale(float timescale)	{ timerTimeScale=timescale; }
	static float getTimeScale()                 { return timerTimeScale;    }

private:
	static float	timerFPS;               //frames
	static float	timerDTInSec;			//in sec
	static int		timerDTInMilliSec;		//in milli sec
	static float	timerElapsedTimeInSec;	//in sec
	static double	timerPreviousTime;      //stored for internal purpose
    
    static float    timerAveragingTime;     //calculate after this much amount of secs
    static double   timerLastTime;          //for internal calculations of averaging fps
    static int      timerFrameCount;        //frame counter

	static float	timerTimeScale;         //global time scale for the engine (especially for shaders)
};
