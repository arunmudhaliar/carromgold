#pragma once

//
//  geTexture.h
//  Copyright 2017 GEAR3D
//  Gear3D
//
//  Created by Arun Mudaliar.
//
//

#include "steTexturePacket.h"

class geTexture {
public:
    enum ETEXTURE {
        TEX_NORMAL,
        TEX_ALPHA,
        TEX_UNDEFINED
    };
    
    geTexture() {
        texture = NULL;
        textureType = TEX_UNDEFINED;
    }
    
    ~geTexture() {
    }
    
    void setTexturePack(steTexturePacket* pTexTurePtr) {
        texture = pTexTurePtr;
    }
    
    steTexturePacket* getTexturePack() const {
        return texture;
    }
    
    unsigned int getTextureID() const {
        return (texture) ? texture->textureID : 0;
    }
    
    void setTextureType(ETEXTURE type){
        textureType = type;
    }
    
    ETEXTURE getTextureType() const {
        return textureType;
    }

private:
    steTexturePacket* texture;     // must not delete this pointer
    ETEXTURE textureType;
};
