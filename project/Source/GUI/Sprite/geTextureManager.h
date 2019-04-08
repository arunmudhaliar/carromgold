#pragma once

//#include "../renderer/glincludes.h"

#include <string.h>
#include <vector>
//#include "types.h"
#include "steTexturePacket.h"
#include "../../Physics/core/types.h"

class CGETextureManager
{
public:
	CGETextureManager();
	~CGETextureManager();

	steTexturePacket*	LoadTexture(const char* aFileName, const char* aOpFileName=NULL);
	void	Reset();	
    void    ReLoad();   //reload all textures from physical memory
    
    unsigned int getTotalTextureMemory()    {   return totalTextureMemory;   }
    
private:
	std::vector<steTexturePacket*>	texturePacketList;
    unsigned int totalTextureMemory;
};
