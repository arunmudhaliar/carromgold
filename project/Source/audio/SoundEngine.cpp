//
//  SoundEngine.cpp
//  testOpenAL
//
//  Created by arun on 31/05/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "SoundEngine.h"
#include "../GUI/OSUtils.h"
//#include "../engine/util/objC_util.h"
//#include "../engine/core/types.h"

#if defined(USE_OPENSL)
#define check(result) check2(result, __LINE__)

void check2(SLresult result, int line)
{
    if (SL_RESULT_SUCCESS != result)
    {        
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("error %s at line %d\n", result, line);
#endif 
        exit(EXIT_FAILURE);
    }
}

#endif

SoundEngine::SoundEngine()
{
#if defined(USE_OPENAL)
    m_pDevice=NULL;
    m_pContext=NULL;
#elif defined(USE_OPENSL)
    m_pEngineObject = NULL;
    m_pEngineEngine = NULL;
    m_pOutputMixObject = NULL;
    listener = NULL;
    listenerLocation=NULL;
#endif

    m_bPaused=false;
    //m_pFileDescriptorPtr=NULL;
}

SoundEngine::~SoundEngine()
{
    destroy();
}

void SoundEngine::init(/*FileDesriptor* fd*/)
{
    //m_pFileDescriptorPtr=fd;
    
#if LOG_DEBUG_ENGINE
    DEBUG_PRINT("Sound Engine Initializing...");
#endif 
    // Initialization

#if defined(USE_OPENAL)
	m_pDevice = alcOpenDevice(NULL); // select the "preferred device"
	if (m_pDevice)
    {
		// use the device to make a context
		m_pContext=alcCreateContext(m_pDevice, NULL);
		if(m_pContext==NULL)
		{			
#if LOG_DEBUG_ENGINE
            DEBUG_PRINT("m_pContext returned NULL");
#endif 
		}
		// set my context to the currently active one
		alcMakeContextCurrent(m_pContext);
	}
	else
	{		
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("alcOpenDevice returned NULL");
#endif 
	}
#elif defined(USE_OPENSL)
	createOpenSLEngine();
#endif

//	createListener();
}

void SoundEngine::reset()
{    
#if LOG_DEBUG_ENGINE
    DEBUG_PRINT("SoundEngine::reset()");
#endif 
    for(int x=0;x<m_pszSources.size();x++)
    {
        SoundSource* soundSrc=m_pszSources[x];
        soundSrc->stop();
        GX_DELETE(soundSrc);
    }
    m_pszSources.clear();
    
#if LOG_DEBUG_ENGINE
    DEBUG_PRINT("Sound sources destroyed...");
#endif 

    for(int x=0;x<m_pszSamples.size();x++)
    {
        SoundSample* sample=m_pszSamples[x];        
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("destrying sound %s", sample->getName().c_str());
#endif 
        GX_DELETE(sample);
    }
    m_pszSamples.clear();
    
    m_bPaused=false;
    
#if LOG_DEBUG_ENGINE
    DEBUG_PRINT("Sound samples destroyed...");
#endif 
}

void SoundEngine::destroyEngine()
{    
#if LOG_DEBUG_ENGINE
    DEBUG_PRINT("SoundEngine::destroy()");
#endif 
#if defined(USE_OPENAL)
    if(m_pContext)
    {	// destroy the context
        alcDestroyContext(m_pContext);
        m_pContext=NULL;
    }
    if(m_pDevice)
	{
        // close the device
        alcCloseDevice(m_pDevice);
        m_pDevice=NULL;
    }    
#if LOG_DEBUG_ENGINE
    DEBUG_PRINT("openAL destroyed");
#endif 
#elif defined(USE_OPENSL)
    
    /* Destroy the listener object */
    if(listener)
    {
    	(*listener)->Destroy(listener);
    	listener=NULL;
    }
    
    // destroy output mix object
    if (m_pOutputMixObject != NULL)
    {
    	(*m_pOutputMixObject)->Destroy(m_pOutputMixObject);
    	m_pOutputMixObject = NULL;
    }
    
    // destroy engine object, and invalidate all associated interfaces
    if (m_pEngineObject != NULL)
    {
        (*m_pEngineObject)->Destroy(m_pEngineObject);
        m_pEngineObject = NULL;
        m_pEngineEngine = NULL;
    }
#endif
}

void SoundEngine::destroyContext()       //only for openAL, call or alarm interrupts
{
#if defined(USE_OPENAL)
    alcMakeContextCurrent (NULL);
    alcSuspendContext(m_pContext);
#endif
}

void SoundEngine::makeContextCurrent()   //only for openAL, call or alarm interrupts
{
#if defined(USE_OPENAL)
    alcMakeContextCurrent(m_pContext);
    alcProcessContext(m_pContext);
#endif
}

void SoundEngine::destroy()
{
    reset();
    destroyEngine();
}

void SoundEngine::pause()
{
    for(int x=0;x<m_pszSources.size();x++)
    {
        SoundSource* soundSrc=m_pszSources[x];
        if(soundSrc->isPlaying())
        {            
#if LOG_DEBUG_ENGINE
            DEBUG_PRINT("pausing %s", soundSrc->getSamplePtr()->getName().c_str());
#endif 
            soundSrc->pause();
        }
    }
    m_bPaused=true;    
#if LOG_DEBUG_ENGINE
   DEBUG_PRINT("Sound Engine Paused");
#endif 
}

void SoundEngine::resume()
{
    for(int x=0;x<m_pszSources.size();x++)
    {
        SoundSource* soundSrc=m_pszSources[x];
        if(soundSrc->isPaused())
            soundSrc->resume();
    }
    m_bPaused=false;    
#if LOG_DEBUG_ENGINE
    DEBUG_PRINT("Sound Engine Resumed");
#endif 
}

SoundSource* SoundEngine::load(const std::string& filename)
{
    std::string orig_filename;
    orig_filename = filename;
    if(filename[0]=='.')
    {
        orig_filename = cpp_getCurrentWorkingDirectory();
        orig_filename+=&filename[1];
    }
    
#if LOG_DEBUG_ENGINE
    DEBUG_PRINT("sound file - %s", orig_filename.c_str());
#endif 

    //search if the sound already loaded
    for(int x=0;x<m_pszSamples.size();x++)
    {
        SoundSample* sample=m_pszSamples[x];
		if(sample->getName().compare(orig_filename)==0)
        {            
#if LOG_DEBUG_ENGINE
            DEBUG_PRINT("sound file - %s already loaded", orig_filename.c_str());
#endif 
            SoundSource* sourceFound=m_pszSources[x];
            sourceFound->resetFlags();
            return sourceFound;
        }
    }
    //

    
    gxFile file;
#if 0
    if(m_pFileDescriptorPtr)
    {
        stFD* fd=m_pFileDescriptorPtr->getFD(orig_filename.c_str());
		file.OpenFileDescriptor(fd->fd, gxFile::FILE_r);
		file.Seek(fd->offset, SEEK_SET);
    }
    else
#endif
    {
        file.OpenFile(orig_filename.c_str());
    }

    
    SoundSample* sample=new SoundSample(orig_filename.c_str());
#if defined(USE_OPENAL)
    sample->loadFromFile(file);
#elif defined(USE_OPENSL)
    sample->loadFromFile(file, m_pEngineEngine, m_pOutputMixObject);
#endif
    m_pszSamples.push_back(sample);
    file.CloseFile();
    //
    
    SoundSource* snd_source=new SoundSource();
    snd_source->createSource(sample, false);
//    snd_source->play();
    
    m_pszSources.push_back(snd_source);
    
    return snd_source;
}

SoundSource* SoundEngine::createInstanceOf(SoundSource* source)
{
    SoundSource* snd_source=new SoundSource();
    snd_source->createSource(source->getSamplePtr(), true);
    m_pszSources.push_back(snd_source);
    
    return snd_source;
}

bool SoundEngine::destroySoundSource(SoundSource* source)
{
    if(!source) return false;
    
    SoundSample* sample=NULL;
    for(int x=0;x<m_pszSources.size();x++)
    {
        SoundSource* soundSrc=m_pszSources[x];

        if(soundSrc==source)
        {
            soundSrc->stop();
            sample=soundSrc->getSamplePtr();
        }
    }
    
    if(sample)
    {
		m_pszSources.erase(std::remove(m_pszSources.begin(), m_pszSources.end(), source), m_pszSources.end());
		m_pszSamples.erase(std::remove(m_pszSamples.begin(), m_pszSamples.end(), sample), m_pszSamples.end());
        //m_pszSources.remove(source);
        //m_pszSamples.remove(sample);
        
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("SoundSource %d destroyed", source->getSourceID());
#endif 
        GX_DELETE(source);
        GX_DELETE(sample);

        return true;
    }
    else
    {
        source->stop();
        m_pszSources.erase(std::remove(m_pszSources.begin(), m_pszSources.end(), source), m_pszSources.end());
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("SoundSource %d is an instance with out sample destroyed", source->getSourceID());
#endif 
        GX_DELETE(source);
        return true;
    }
    

    return false;
}

void SoundEngine::setListenerPos(float x, float y, float z)
{
#if defined(USE_OPENAL)
    ALfloat listenerPos[]={x, y, z};
    alListenerfv(AL_POSITION, listenerPos);
#elif defined(USE_OPENSL)
    if(listenerLocation)
    {
    	SLVec3D coords;
    	coords.x=x;        coords.y=y;        coords.z=z;
    	(*listenerLocation)->SetLocationCartesian(listenerLocation, &coords);
    }
#endif
}

void SoundEngine::setListenerOrientation(float atX, float atY, float atZ, float upX, float upY, float upZ)
{
#if defined(USE_OPENAL)
    // Orientation of the Listener. (first 3 elements are "at", second 3 are "up")
    ALfloat ListenerOri[] = { atX, atY, atZ,  upX, upY, upZ };
    alListenerfv(AL_ORIENTATION, ListenerOri);
#endif
}

void SoundEngine::setListenerVolume(float volume)
{
#if defined(USE_OPENAL)
    alListenerf(AL_GAIN, volume);
#endif
}

#define MAX_NUMBER_LISTENER_INTERFACES 4
void SoundEngine::createListener()
{
#if defined(USE_OPENSL)
	/* Create 3D listener - no interfaces requires as the listener will
	remain stationary */
	SLboolean required[MAX_NUMBER_LISTENER_INTERFACES];
	SLInterfaceID iidArray[MAX_NUMBER_LISTENER_INTERFACES];
	for (int i=0;i<MAX_NUMBER_LISTENER_INTERFACES;i++)
	{
		required[i] = SL_BOOLEAN_FALSE;
		iidArray[i] = SL_IID_NULL;
	}    
#if LOG_DEBUG_ENGINE
    DEBUG_PRINT("opensl 5");
#endif 
	SLresult result = (*m_pEngineEngine)->CreateListener(m_pEngineEngine, &listener, 0, iidArray, required);
    check(result);
    
#if LOG_DEBUG_ENGINE
    DEBUG_PRINT("opensl 6");
#endif 
    /* Realizing the listener object in synchronous mode. */
    result = (*listener)->Realize(listener, SL_BOOLEAN_FALSE);
    check(result);
    
#if LOG_DEBUG_ENGINE
    DEBUG_PRINT("opensl 7");
#endif 
    result = (*listener)->GetInterface(listener, SL_IID_3DLOCATION, (void *)&listenerLocation);
    check(result);
    
#if LOG_DEBUG_ENGINE
    DEBUG_PRINT("opensl 8");
#endif 
    SLVec3D coords;
    coords.x=0;        coords.y=0;        coords.z=0;
    (*listenerLocation)->SetLocationCartesian(listenerLocation, &coords);
    
#if LOG_DEBUG_ENGINE
    DEBUG_PRINT("opensl 9");
#endif 
#endif
}

#if defined(USE_OPENSL)
// Same as above but automatically adds the source code line number

void SoundEngine::createOpenSLEngine()
{
    SLresult result;

//    const SLInterfaceID engine_ids[] = {SL_IID_ENGINE, SL_IID_3DCOMMIT};
//    const SLboolean engine_req[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    const SLInterfaceID engine_ids[] = { SL_IID_ENGINE, SL_IID_3DCOMMIT};
    const SLboolean engine_req[] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_FALSE};
    SLEngineOption engine_options[] = {(SLuint32) SL_ENGINEOPTION_THREADSAFE, (SLuint32) SL_BOOLEAN_TRUE};

    result = slCreateEngine(&m_pEngineObject, 1, engine_options, 2, engine_ids, engine_req);
    check(result);
    
#if LOG_DEBUG_ENGINE
   DEBUG_PRINT("opensl 0");
#endif 
    // realize the engine
    result = (*m_pEngineObject)->Realize(m_pEngineObject, SL_BOOLEAN_FALSE);
    check(result);
    
#if LOG_DEBUG_ENGINE
    DEBUG_PRINT("opensl 1");
#endif 
    // get the engine interface, which is needed in order to create other objects
    result = (*m_pEngineObject)->GetInterface(m_pEngineObject, SL_IID_ENGINE, &m_pEngineEngine);
    check(result);
    
#if LOG_DEBUG_ENGINE
    DEBUG_PRINT("opensl 2");
#endif 
    // create output mix, with environmental reverb specified as a non-required interface
    const SLInterfaceID ids[] = {SL_IID_NULL, SL_IID_VOLUME};	//SL_IID_ENVIRONMENTALREVERB
    const SLboolean req[] = {SL_BOOLEAN_FALSE, SL_BOOLEAN_FALSE};

    result = (*m_pEngineEngine)->CreateOutputMix(m_pEngineEngine, &m_pOutputMixObject, 2, ids, req);
    check(result);
    
#if LOG_DEBUG_ENGINE
    DEBUG_PRINT("opensl 3");
#endif 
    // realize the output mix
    result = (*m_pOutputMixObject)->Realize(m_pOutputMixObject, SL_BOOLEAN_FALSE);
    check(result);
    
#if LOG_DEBUG_ENGINE
    DEBUG_PRINT("opensl 4");
#endif 

//    SLVolumeItf volumeItf;
//    result = (*m_pOutputMixObject)->GetInterface(m_pOutputMixObject, SL_IID_VOLUME, (void*)&volumeItf);
//    check(result);
//    if(volumeItf==NULL)
//    {
//    	DEBUG_PRINT("SL_IID_VOLUME not supported");
//    }


    //device caps
    SLuint32 nextensions=0;
    (*m_pEngineEngine)->QueryNumSupportedExtensions(m_pEngineEngine, &nextensions);
    
#if LOG_DEBUG_ENGINE
    DEBUG_PRINT("No : of openSL extensions supported = %d", nextensions);
#endif 
    for(int x=0;x<nextensions;x++)
    {
    	SLint16 len=0;
    	(*m_pEngineEngine)->QuerySupportedExtension(m_pEngineEngine, x, NULL, &len);
    	SLchar name[len];
    	(*m_pEngineEngine)->QuerySupportedExtension(m_pEngineEngine, x, name, &len);
    	
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("openSL extension - %s (length=%d)", name, len);
#endif 
    }

//#define SL_OBJECTID_ENGINE			((SLuint32) 0x00001001)
//#define SL_OBJECTID_LEDDEVICE		((SLuint32) 0x00001002)
//#define SL_OBJECTID_VIBRADEVICE		((SLuint32) 0x00001003)
//#define SL_OBJECTID_AUDIOPLAYER		((SLuint32) 0x00001004)
//#define SL_OBJECTID_AUDIORECORDER	((SLuint32) 0x00001005)
//#define SL_OBJECTID_MIDIPLAYER		((SLuint32) 0x00001006)
//#define SL_OBJECTID_LISTENER		((SLuint32) 0x00001007)
//#define SL_OBJECTID_3DGROUP			((SLuint32) 0x00001008)
//#define SL_OBJECTID_OUTPUTMIX		((SLuint32) 0x00001009)
//#define SL_OBJECTID_METADATAEXTRACTOR	((SLuint32) 0x0000100A)

    SLuint32 nInterfaces=0;
    result = (*m_pEngineEngine)->QueryNumSupportedInterfaces(m_pEngineEngine, SL_OBJECTID_ENGINE, &nInterfaces);
    if(SL_RESULT_SUCCESS == result)
    {
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("SL_OBJECTID_ENGINE supported with %d interfaces", nInterfaces);
#endif     	
    }
    result = (*m_pEngineEngine)->QueryNumSupportedInterfaces(m_pEngineEngine, SL_OBJECTID_LEDDEVICE, &nInterfaces);
    if(SL_RESULT_SUCCESS == result)
    {
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("SL_OBJECTID_LEDDEVICE supported with %d interfaces", nInterfaces);
#endif     	
    }
    result = (*m_pEngineEngine)->QueryNumSupportedInterfaces(m_pEngineEngine, SL_OBJECTID_VIBRADEVICE, &nInterfaces);
    if(SL_RESULT_SUCCESS == result)
    {
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("SL_OBJECTID_VIBRADEVICE supported with %d interfaces", nInterfaces);
#endif     	
    }
    result = (*m_pEngineEngine)->QueryNumSupportedInterfaces(m_pEngineEngine, SL_OBJECTID_AUDIOPLAYER, &nInterfaces);
    if(SL_RESULT_SUCCESS == result)
    {
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("SL_OBJECTID_AUDIOPLAYER supported with %d interfaces", nInterfaces);
#endif     	
    }
    result = (*m_pEngineEngine)->QueryNumSupportedInterfaces(m_pEngineEngine, SL_OBJECTID_AUDIORECORDER, &nInterfaces);
    if(SL_RESULT_SUCCESS == result)
    {
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("SL_OBJECTID_AUDIORECORDER supported with %d interfaces", nInterfaces);
#endif     	
    }
    result = (*m_pEngineEngine)->QueryNumSupportedInterfaces(m_pEngineEngine, SL_OBJECTID_MIDIPLAYER, &nInterfaces);
    if(SL_RESULT_SUCCESS == result)
    {
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("SL_OBJECTID_MIDIPLAYER supported with %d interfaces", nInterfaces);
#endif     	
    }
    result = (*m_pEngineEngine)->QueryNumSupportedInterfaces(m_pEngineEngine, SL_OBJECTID_LISTENER, &nInterfaces);
    if(SL_RESULT_SUCCESS == result)
    {
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("SL_OBJECTID_LISTENER supported with %d interfaces", nInterfaces);
#endif     	
    }
    result = (*m_pEngineEngine)->QueryNumSupportedInterfaces(m_pEngineEngine, SL_OBJECTID_3DGROUP, &nInterfaces);
    if(SL_RESULT_SUCCESS == result)
    {
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("SL_OBJECTID_3DGROUP supported with %d interfaces", nInterfaces);
#endif 
    	
    }
    result = (*m_pEngineEngine)->QueryNumSupportedInterfaces(m_pEngineEngine, SL_OBJECTID_OUTPUTMIX, &nInterfaces);
    if(SL_RESULT_SUCCESS == result)
    {
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("SL_OBJECTID_OUTPUTMIX supported with %d interfaces", nInterfaces);
#endif 
    	
    }
    result = (*m_pEngineEngine)->QueryNumSupportedInterfaces(m_pEngineEngine, SL_OBJECTID_METADATAEXTRACTOR, &nInterfaces);
    if(SL_RESULT_SUCCESS == result)
    {
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("SL_OBJECTID_METADATAEXTRACTOR supported with %d interfaces", nInterfaces);
#endif 
    	
    }
    //DEBUG_PRINT("No : of openSL interfaces supported = %d", nInterfaces);
    //
}
#endif
