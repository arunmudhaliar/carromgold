#include "geTextureManager.h"
#ifndef GEAR2D
#include "pngLoader.h"
#else
#include "../../../../GEAR2D_Alpha/GEAR2D/app/pngLoaderfor2D.h"
#endif

CGETextureManager::CGETextureManager()
{
    totalTextureMemory=0;
}

CGETextureManager::~CGETextureManager()
{
	Reset();
}

steTexturePacket* CGETextureManager::LoadTexture(const char* aFileName, const char* aOpFileName)
{
	if(aFileName && aOpFileName)	//if both textures are there
	{
		int i=0;
		for(i=0;i<(int)texturePacketList.size();i++)
		{
			if(texturePacketList[i]->textureName && (strcmp(aFileName, texturePacketList[i]->textureName)==0))
			{
				if(texturePacketList[i]->alphaTextureName && (strcmp(aOpFileName, texturePacketList[i]->alphaTextureName)==0))
					break;
			}
		}
		if(i!=(int)texturePacketList.size())	return texturePacketList[i];	//if texture already exist in list
	}
	else 
	{
		if(aFileName)
		{
			int i=0;
			for(i=0;i<(int)texturePacketList.size();i++)
			{
				if(texturePacketList[i]->textureName && (strcmp(aFileName, texturePacketList[i]->textureName)==0))
				{
					if(!texturePacketList[i]->alphaTextureName)
						break;
				}
			}
			if(i!=(int)texturePacketList.size())	return texturePacketList[i];	//if texture already exist in list
		}//only diffuse
		else if(aOpFileName)
		{
			int i=0;
			for(i=0;i<(int)texturePacketList.size();i++)
			{
				if(texturePacketList[i]->alphaTextureName && (strcmp(aOpFileName, texturePacketList[i]->alphaTextureName)==0))
				{
					if(!texturePacketList[i]->textureName)
						break;
				}
			}
			if(i!=(int)texturePacketList.size())	return texturePacketList[i];	//if texture already exist in list
		}// only optex
	} // if there is only one tex

	steTexturePacket*	aNewTexturePacket	= new steTexturePacket();
	
	unsigned int texID=0;
	bool alpha_tex=false;

    //texture loading utility
	texID=read_png_file(aFileName, alpha_tex, aNewTexturePacket->m_cWidth, aNewTexturePacket->m_cHeight, aNewTexturePacket->m_cBpp);
    aNewTexturePacket->isAlphaTexure=alpha_tex;

	if(texID==0)
	{
		printf("%s not found", aFileName);
		GX_DELETE(aNewTexturePacket);
		return NULL;
	}

    if(texID>0)
    {
        totalTextureMemory+=(unsigned int)(aNewTexturePacket->m_cWidth*aNewTexturePacket->m_cHeight*aNewTexturePacket->m_cBpp);
    }

	if(aFileName)
	{
		aNewTexturePacket->textureName = new char[strlen(aFileName)+1];
		strcpy(aNewTexturePacket->textureName, aFileName);
		aNewTexturePacket->textureName[strlen(aFileName)]	= '\0';
	}

	if(aOpFileName)
	{
		aNewTexturePacket->alphaTextureName = new char[strlen(aOpFileName)+1];
		strcpy(aNewTexturePacket->alphaTextureName, aOpFileName);
		aNewTexturePacket->alphaTextureName[strlen(aOpFileName)]	= '\0';
	}

	aNewTexturePacket->isAlphaTexure	= alpha_tex;
	aNewTexturePacket->textureID	= texID;

	texturePacketList.push_back(aNewTexturePacket);
	
	return aNewTexturePacket;
}

void CGETextureManager::ReLoad()
{
}

void CGETextureManager::Reset()
{
	for(int i=0;i<texturePacketList.size();i++)
	{
		steTexturePacket* packet=texturePacketList[i];
		GX_DELETE(packet);
	}
	texturePacketList.clear();
    totalTextureMemory=0;
}
