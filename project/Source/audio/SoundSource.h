#pragma once

#include "../Physics/core/types.h"

#if defined(USE_OPENAL)
	#include <OpenAL/al.h>
	#include <OpenAL/alc.h>
#elif defined(USE_OPENSL)
	#include <SLES/OpenSLES.h>
	#include <SLES/OpenSLES_Android.h>
#endif

#if defined(USE_OPENAL)
	#include <OpenAL/al.h>
	#include <OpenAL/alc.h>
#elif defined(USE_OPENSL)
	#include <SLES/OpenSLES.h>
	#include <SLES/OpenSLES_Android.h>
#endif

#include "SoundSample.h"

#if defined(USE_OPENSL)
    void bqPlayerCallback(SLBufferQueueItf bq, void *context);
#endif

class SoundSource
{
public:
    SoundSource();    
    ~SoundSource();
    
    unsigned int getSourceID()              {   return m_iSourceId;     }
    //void setSourceID(unsigned int srcId)    {   m_iSourceId=srcId;      }
    
    void createSource(SoundSample* sample, bool bInstance);
    
    void play(bool bLoop=true);
    void stop();
    
	void pause();
	void resume();
	
    void setPosition(int x, int y, int z)
    {
#if defined(USE_OPENAL)
        alSource3f(m_iSourceId, AL_POSITION, x, y, z);
#elif defined(USE_OPENSL)
        SL3DLocationItf locationIntf=m_pSoundSamplePtr->getLocationInterface();
        if(locationIntf)
        {
        	SLVec3D coords;
        	coords.x=x;        coords.y=y;        coords.z=z;
        	(*locationIntf)->SetLocationCartesian(locationIntf, &coords);
        }
#endif
    }

    float gain_to_attenuation( float gain )
    {
        return gain < 0.01F ? -96.0F : 20 * log10( gain );
    }

    void setVolume(float scale) //0 to 1
    {
#if defined(USE_OPENAL)
        alSourcef(m_iSourceId, AL_GAIN, scale);
#elif defined(USE_OPENSL)
//        scale = 0.875f;
        scale = 0.85f + scale*(1.0f-0.85f);
        //get min & max
        SLVolumeItf volumeIntf=m_pSoundSamplePtr->getVolumeInterface();
        SLmillibel MinVolume = SL_MILLIBEL_MIN;
        SLmillibel MaxVolume = SL_MILLIBEL_MIN;
        (*volumeIntf)->GetMaxVolumeLevel(volumeIntf, &MaxVolume);
        //calc SLES volume
        SLmillibel Volume = MinVolume + (SLmillibel)( ((float)(MaxVolume - MinVolume))*scale );

//        DEBUG_PRINT("arun : Volume %d, %d, %d", MinVolume, MaxVolume, (SLmillibel)gain_to_attenuation(scale)*100);
        //set
//        int dBVolume = 20* log2f(scale)/log2(10);

//        DEBUG_PRINT("arun : dBVolume %f", mB);

//        Volume = (SLmillibel)(1000.0f * 20.0f * log10f(scale));
        (*volumeIntf)->SetVolumeLevel(volumeIntf, Volume);
#endif
    }
    
    void setPitch(float scale)  //0 to 1
    {
#if defined(USE_OPENAL)
        alSourcef(m_iSourceId, AL_PITCH, scale);
#endif
    }
    
    void setReferenceDistance(float distance)
    {
#if defined(USE_OPENAL)
        alSourcef(m_iSourceId, AL_REFERENCE_DISTANCE, distance);
#endif
    }
    
    void setMaxDistance(float distance)
    {
#if defined(USE_OPENAL)
        alSourcef(m_iSourceId, AL_MAX_DISTANCE, distance);
#endif
    }
    
    bool isPlaying()    
    {
#if defined(USE_OPENAL)
        ALint state;
        alGetSourcei(m_iSourceId, AL_SOURCE_STATE, &state);
        return ((state == AL_PLAYING) || (state == AL_LOOPING));
#elif defined(USE_OPENSL)
        //GetPlayState
        SLresult result;
        SLuint32 state;
        result = (*m_pSoundSamplePtr->getPlayInterface())->GetPlayState(m_pSoundSamplePtr->getPlayInterface(), &state);

        return ((state==SL_PLAYSTATE_PLAYING));
#endif
        return false;
    }
    
    void resetFlags()   {   m_bPaused=false;    }
    bool isPaused()     {   return m_bPaused;   }
    
    SoundSample* getSamplePtr()   {   return m_pSoundSamplePtr;   }
#if defined(USE_OPENSL)
    bool isLoopable()	{	return m_bLoop;	}
#endif
    
private:
    unsigned int m_iSourceId;
    bool m_bPaused;
    SoundSample* m_pSoundSamplePtr; //must not delete this pointer

#if defined(USE_OPENSL)
    bool m_bLoop;	//used for android
#endif
};
