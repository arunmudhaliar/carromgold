//
//  SoundSample.cpp
//  testOpenAL
//
//  Created by arun on 31/05/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "SoundSample.h"
//#include "../engine/util/objC_util.h"
#include "../zlib-1.2.11/zlib.h"

SoundSample::SoundSample(const char* filename)
{
    m_iBufferId=0;
    m_iFrequency=0;
    m_iFormat=0;
    m_iSize=0;
    if(filename)
    {
        m_cszName.assign(filename, strlen(filename));
    }

#if defined(USE_OPENSL)
    m_pQueue = NULL;
    m_pPlayer = NULL;
    m_pPlayerPlay = NULL;
    m_pVolume = NULL;
    m_pLocationItf=NULL;
#endif
}

SoundSample::~SoundSample()
{
#if defined(USE_OPENAL)
    alDeleteBuffers(1, &m_iBufferId);
#elif defined(USE_OPENSL)
    if (m_pPlayer != NULL)
    {
        (*m_pPlayer)->Destroy(m_pPlayer);
        m_pPlayer = NULL;
        m_pPlayerPlay = NULL;
        m_pQueue = NULL;
        m_pLocationItf=NULL;
    }

    free(m_pRawSoundBuffer);
#endif
}

#if defined(USE_OPENAL) || defined(USE_OPENSL)
#if defined(USE_OPENAL)
bool SoundSample::loadFromFile(gxFile& file)
#elif defined(USE_OPENSL)
bool SoundSample::loadFromFile(gxFile& file, const SLEngineItf engineEngine, const SLObjectItf outputMixObject)
#endif
{    
    int signature;
    file.Read(signature);
    file.Read(m_iFormat);
    file.Read(m_iFrequency);
    file.Read(m_iSize);             //uncompressed size
    uint32_t compressedSz=0;
    file.Read(compressedSz);        //compressed size
    
    unsigned char* compressed_buffer=(unsigned char*)malloc(compressedSz);
    file.ReadBuffer(compressed_buffer, compressedSz);
    
    unsigned char* outData = (unsigned char*)malloc(m_iSize);
    unsigned long uncompressed_sz=m_iSize;
    int ret=uncompress(outData, &uncompressed_sz, compressed_buffer, compressedSz);
    if(ret!=Z_OK)
    {
        free(compressed_buffer);
        free(outData);
        compressed_buffer=NULL;
        outData=NULL;
        DEBUG_PRINT("SoundSample LOAD ERROR !!!");
        //DEBUG_ASSERT(true, "data uncompression error");
    }

#if defined(USE_OPENAL)
    alGenBuffers(1, &m_iBufferId);    
    // jam the audio data into the new buffer
    alBufferData(m_iBufferId, m_iFormat, outData, m_iSize, m_iFrequency);
    int err=alGetError();
    if(err!=0)
    {
#if defined (LOG_DEBUG_ENGINE)
        DEBUG_PRINT("openAL ERROR %d at loadFromFile()", err);
#endif        
    }
    
#elif defined(USE_OPENSL)
    int nChannels=1;
    SLuint32 channelMask=SL_SPEAKER_FRONT_CENTER;
    SLuint16 bitrate=SL_PCMSAMPLEFORMAT_FIXED_16;	//default
    switch (m_iFormat)
    {
    	case 0x1100:	//AL_FORMAT_MONO8
    	{
    		bitrate=SL_PCMSAMPLEFORMAT_FIXED_8;
#if defined (LOG_DEBUG_ENGINE)
            DEBUG_PRINT("SL_PCMSAMPLEFORMAT_FIXED_8 - mono");
#endif    		
    	}
    	break;
    	case 0x1101:	//AL_FORMAT_MONO16
    	{
    		bitrate=SL_PCMSAMPLEFORMAT_FIXED_16;
#if defined (LOG_DEBUG_ENGINE)
            DEBUG_PRINT("SL_PCMSAMPLEFORMAT_FIXED_16 - mono");	//ok
#endif
    		
    	}
    	break;
    	case 0x1102:	//AL_FORMAT_STEREO8
    	{
    		bitrate=SL_PCMSAMPLEFORMAT_FIXED_8;
    		nChannels=2;
    		channelMask=SL_SPEAKER_FRONT_LEFT|SL_SPEAKER_FRONT_RIGHT;
#if defined (LOG_DEBUG_ENGINE)
            DEBUG_PRINT("SL_PCMSAMPLEFORMAT_FIXED_8 - stereo");
#endif    		
    	}
    	break;
    	case 0x1103:	//AL_FORMAT_STEREO16
    	{
    		bitrate=SL_PCMSAMPLEFORMAT_FIXED_16;
    		nChannels=2;
    		channelMask=SL_SPEAKER_FRONT_LEFT|SL_SPEAKER_FRONT_RIGHT;
#if defined (LOG_DEBUG_ENGINE)
            DEBUG_PRINT("SL_PCMSAMPLEFORMAT_FIXED_16 - stereo");
#endif    		
    	}
    	break;
    }

    SLuint32 samplingRate = (SLuint32)(m_iFrequency*1000);
    SLresult result;

    // configure audio source
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, (unsigned int)nChannels, samplingRate,
        bitrate, bitrate,
        channelMask, SL_BYTEORDER_LITTLEENDIAN};

    SLDataSource audioSrc = {&loc_bufq, &format_pcm};
#if defined (LOG_DEBUG_ENGINE)
    DEBUG_PRINT("sample load 0");
#endif
    
    // create audio player
    const SLInterfaceID player_ids[] = {SL_IID_BUFFERQUEUE, SL_IID_PLAY, SL_IID_VOLUME, SL_IID_3DLOCATION};
    const SLboolean player_req[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_FALSE};

    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    //SLVolumeItf volume;
    //LOGI("Creating SLAndroidSimpleBufferQueueItf");

    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &m_pPlayer, &audioSrc, &audioSnk,
        4, player_ids, player_req);
//    check(result);
#if defined (LOG_DEBUG_ENGINE)
    DEBUG_PRINT("sample load 1");
#endif    

    // realize the player
    result = (*m_pPlayer)->Realize(m_pPlayer, SL_BOOLEAN_FALSE);
    //check(result);
#if defined (LOG_DEBUG_ENGINE)
    DEBUG_PRINT("sample load 2");
#endif   

    // get the play interface
    result = (*m_pPlayer)->GetInterface(m_pPlayer, SL_IID_PLAY, &m_pPlayerPlay);
    //check(result);
#if defined (LOG_DEBUG_ENGINE)
    DEBUG_PRINT("sample load 3");
#endif
    

    // get the buffer queue interface
    result = (*m_pPlayer)->GetInterface(m_pPlayer, SL_IID_BUFFERQUEUE, &m_pQueue);
    //check(result);
#if defined (LOG_DEBUG_ENGINE)
    DEBUG_PRINT("sample load 4");
#endif    

    // register callback on the buffer queue
    //result = (*queue)->RegisterCallback(queue, OpenSLSoundPool::staticBqPlayerCallback, NULL);
    //check(result);

    result = (*m_pPlayer)->GetInterface(m_pPlayer, SL_IID_VOLUME, &m_pVolume);
    //check(result);
#if defined (LOG_DEBUG_ENGINE)
   DEBUG_PRINT("sample load 5");
#endif    

    result = (*m_pPlayer)->GetInterface(m_pPlayer, SL_IID_3DLOCATION, (void*)&m_pLocationItf);    
#if defined (LOG_DEBUG_ENGINE)
    DEBUG_PRINT("sample load 6");
#endif
    if(m_pLocationItf==NULL)
    {
    	
#if defined (LOG_DEBUG_ENGINE)
       DEBUG_PRINT("SL_IID_3DLOCATION not supported");
#endif
    }

    result = (*m_pPlayerPlay)->SetPlayState(m_pPlayerPlay, SL_PLAYSTATE_STOPPED);
    
#if defined (LOG_DEBUG_ENGINE)
    DEBUG_PRINT("sample loaded");
#endif
//    SLmillibel pMaxLevel=0;
//    result = (*m_pVolume)->GetMaxVolumeLevel(m_pVolume, &pMaxLevel);
//    DEBUG_PRINT("max volume level = %d", pMaxLevel);
//    SLmillibel volume=0;
//    result = (*m_pVolume)->GetVolumeLevel(m_pVolume, &volume);
//    DEBUG_PRINT("current volume level = %d", volume);

#endif

    if(outData)
    {
        free(compressed_buffer);
#if defined(USE_OPENAL)
        free(outData);
#elif defined(USE_OPENSL)
        m_pRawSoundBuffer=outData;
#endif
    }
    
#if defined (LOG_DEBUG_ENGINE)
    DEBUG_PRINT("(format=%d, freq=%d, size=%d)", m_iFormat, m_iFrequency, m_iSize);
#endif
    
    
    return true;
}
#endif
