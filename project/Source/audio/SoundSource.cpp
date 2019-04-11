//
//  SoundSource.cpp
//  testOpenAL
//
//  Created by arun on 31/05/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "SoundSource.h"

SoundSource::SoundSource()
{
    m_iSourceId=0;
    m_bPaused=false;
    m_pSoundSamplePtr=NULL;
#if defined(USE_OPENSL)
    m_bLoop=false;	//used for android
#endif
}

SoundSource::~SoundSource()
{
#if defined(USE_OPENAL)
    alDeleteSources(1, &m_iSourceId);
#elif defined(USE_OPENSL)
    m_bLoop=false;	//used for android
    stop();
#endif
    m_pSoundSamplePtr=NULL;
    m_iSourceId=0;
}

void SoundSource::createSource(SoundSample* sample, bool bInstance)
{
//    if(!bInstance)
    {
        m_pSoundSamplePtr=sample;
    }
    
#if defined(USE_OPENAL)
    // grab a source ID from openAL
    alGenSources(1, &m_iSourceId); 
    
    // attach the buffer to the source
    alSourcei(m_iSourceId, AL_BUFFER, sample->getBufferID());
    // set some basic source prefs
    alSourcef(m_iSourceId, AL_PITCH, 1.0f);
    alSourcef(m_iSourceId, AL_GAIN, 1.0f);
    alSourcei(m_iSourceId, AL_LOOPING, AL_TRUE);
    alSource3f(m_iSourceId, AL_POSITION, 0.0f, 0.0f, 0.0f);
    
    int err=alGetError();
    if(err!=0)
    {        
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("openAL ERROR %d on createSource()", err);
#endif
    }

#endif
}

void SoundSource::play(bool bLoop)
{
#if defined(USE_OPENAL)

    alSourceRewind(m_iSourceId);
    alSourcei(m_iSourceId, AL_LOOPING, (bLoop)?AL_TRUE:AL_FALSE);
    alSourcePlay(m_iSourceId);
    
    int err=alGetError();
    if(err!=0)
    {        
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("openAL ERROR %d on Play()", err);
#endif
    }

#elif defined(USE_OPENSL)

    stop();

    if(SL_RESULT_SUCCESS!=(*m_pSoundSamplePtr->getQueueInterface())->Enqueue(m_pSoundSamplePtr->getQueueInterface(), m_pSoundSamplePtr->getRawSoundBuffer(), m_pSoundSamplePtr->getBufferSize()))
    {    	
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("SoundSource...Enqueue failed on play()");
#endif
    	stop();
    	return;
    }

    // register callback on the buffer queue
    if(SL_RESULT_SUCCESS!=(*m_pSoundSamplePtr->getQueueInterface())->RegisterCallback(m_pSoundSamplePtr->getQueueInterface(), bqPlayerCallback, this))
    {    	
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("SoundSource...Enqueue RegisterCallback on play()");
#endif
    	stop();
    	return;
    }
    //check(result);

    if(SL_RESULT_SUCCESS!=(*m_pSoundSamplePtr->getPlayInterface())->SetPlayState(m_pSoundSamplePtr->getPlayInterface(), SL_PLAYSTATE_PLAYING))
    {    	
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("SoundSource...Enqueue SetPlayState on play()");
#endif
    	//stop();
    	//return;
    }
    m_bLoop=bLoop;
#endif
}

void SoundSource::stop()
{
#if defined(USE_OPENAL)
    alSourceStop(m_iSourceId);
#elif defined(USE_OPENSL)
    m_bLoop=false;

	if(SL_RESULT_SUCCESS!=(*m_pSoundSamplePtr->getQueueInterface())->Clear(m_pSoundSamplePtr->getQueueInterface()))
	{		
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("SoundSource...Clear failed");
#endif
	}

    if(SL_RESULT_SUCCESS!=(*m_pSoundSamplePtr->getPlayInterface())->SetPlayState(m_pSoundSamplePtr->getPlayInterface(), SL_PLAYSTATE_STOPPED))
    {    	
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("SoundSource...stop failed");
#endif
    }
#endif
}

void SoundSource::pause()
{
#if defined(USE_OPENAL)
	alSourcePause(m_iSourceId);
#elif defined(USE_OPENSL)
	if(SL_RESULT_SUCCESS!=(*m_pSoundSamplePtr->getPlayInterface())->SetPlayState(m_pSoundSamplePtr->getPlayInterface(), SL_PLAYSTATE_PAUSED))
	{		
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("SoundSource...pause failed");
#endif
	}
#endif
    m_bPaused=true;
}

void SoundSource::resume()
{
#if defined(USE_OPENAL)
	alSourcePlay(m_iSourceId);
#elif defined(USE_OPENSL)
	if(SL_RESULT_SUCCESS!=(*m_pSoundSamplePtr->getPlayInterface())->SetPlayState(m_pSoundSamplePtr->getPlayInterface(), SL_PLAYSTATE_PLAYING))
	{		
#if LOG_DEBUG_ENGINE
       DEBUG_PRINT("SoundSource...resume failed");
#endif
	}
#endif
    m_bPaused=false;
}

#if defined(USE_OPENSL)
void bqPlayerCallback(SLBufferQueueItf bq, void *context)
{
	SoundSource* source=(SoundSource*)context;
	
#if LOG_DEBUG_ENGINE
    DEBUG_PRINT("callback called for %s", source->getSamplePtr()->getName().c_str());
#endif	
	if(source->isLoopable())
	{
	    SLresult result;
		
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("looping...");
#endif	
		if(SL_RESULT_SUCCESS!=(*source->getSamplePtr()->getQueueInterface())->Clear(source->getSamplePtr()->getQueueInterface()))
		{
#if LOG_DEBUG_ENGINE
            DEBUG_PRINT("looping...Clear failed");
#endif				
			source->stop();
			return;
		}

		if(SL_RESULT_SUCCESS!=(*source->getSamplePtr()->getQueueInterface())->Enqueue(source->getSamplePtr()->getQueueInterface(), source->getSamplePtr()->getRawSoundBuffer(), source->getSamplePtr()->getBufferSize()))
		{
#if LOG_DEBUG_ENGINE
            DEBUG_PRINT("looping...Enqueue failed");
#endif				
			source->stop();
			return;
		}

	    // register callback on the buffer queue
		if(SL_RESULT_SUCCESS!=(*source->getSamplePtr()->getQueueInterface())->RegisterCallback(source->getSamplePtr()->getQueueInterface(), bqPlayerCallback, context))
		{
#if LOG_DEBUG_ENGINE
            DEBUG_PRINT("looping...RegisterCallback failed");
#endif				
			//source->stop();
			//return;
		}
	}
}
#endif
