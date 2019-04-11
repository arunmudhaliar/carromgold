#pragma once

//
//  steTexturePacket.h
//  Copyright 2017 GEAR3D
//  Gear3D
//
//  Created by Arun Mudaliar.
//
//

//#include "../renderer/glincludes.h"
//#include "../util/geDefines.h"
#if GEAR_APPLE
    #include <OpenGL/gl.h>
#elif GEAR_ANDROID
    #include <GLES2/gl2.h>
#else
    #include <gl.h>
#endif
#include "../../Physics/core/types.h"
#include "../gxDebug.h"

struct steTexturePacket {
    steTexturePacket() {
        textureID = 0;
        textureName = NULL;
        alphaTextureName = NULL;
        isAlphaTexure = false;
        
        m_cWidth = 0;
        m_cHeight = 0;
        m_cBpp = 0;
    }
    
    void releaseTextureData() {
        if (textureID > 0) {
            glDeleteTextures(1, &textureID);
        }
        textureID = 0;
        isAlphaTexure = false;
        m_cWidth = m_cHeight = m_cBpp = 0;
    }
    
    ~steTexturePacket() {
        releaseTextureData();
        
        if (textureName) {
    #if LOG_DEBUG_ENGINE
            DEBUG_PRINT("unloading texture : %s", textureName);
    #endif
        }
        
        GX_DELETE_ARY(textureName);
        GX_DELETE_ARY(alphaTextureName);
    }
    
    unsigned int textureID;
    char* textureName;
    char* alphaTextureName;
    bool isAlphaTexure;

    unsigned int m_cWidth;
    unsigned int m_cHeight;
    unsigned int m_cBpp;
};
