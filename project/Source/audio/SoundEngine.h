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

#include <stdio.h>
#include <stdlib.h>

#include "SoundSample.h"
#include "SoundSource.h"
#include <vector>
#include <algorithm>
//#include "../engine/core/fdlist.h"

class SoundEngine
{
public:
    SoundEngine();
    ~SoundEngine();
    
    void init(/*FileDesriptor* fd*/);
    void reset();
    void destroyEngine();
    void destroy();
    
    void destroyContext();       //only for openAL, call or alarm interrupts
    void makeContextCurrent();   //only for openAL, call or alarm interrupts
    
	void pause();
	void resume();
	
    SoundSource* load(const std::string& filename);
    
    SoundSource* getSource(int index)   {   return m_pszSources[index];  }
    
    bool destroySoundSource(SoundSource* source);
    
    int getSourceCount()    {   return (int)m_pszSources.size();     }
    
    SoundSource* createInstanceOf(SoundSource* source);
    
    void setListenerPos(float x, float y, float z);
    void setListenerOrientation(float atX, float atY, float atZ, float upX, float upY, float upZ);
    void setListenerVolume(float volume);
    
    void createListener();

    bool isPaused()     {   return m_bPaused;   }
    void resetFlags()   {   m_bPaused=false;    }
private:
#if defined(USE_OPENSL)
    void createOpenSLEngine();
#endif
    
    //FileDesriptor* m_pFileDescriptorPtr;    //must not delete this pointer
    
#if defined(USE_OPENAL)
    ALCcontext* m_pContext;
    ALCdevice* m_pDevice;
#elif defined(USE_OPENSL)
    // engine interfaces
    SLObjectItf m_pEngineObject;
    SLEngineItf m_pEngineEngine;
    // output mix interfaces
    SLObjectItf m_pOutputMixObject;

    SLObjectItf listener;
    SL3DLocationItf listenerLocation;
#endif
    
    bool m_bPaused;
	std::vector<SoundSample*> m_pszSamples;
    std::vector<SoundSource*> m_pszSources;
};
