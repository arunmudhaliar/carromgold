#pragma once

#include "geTextureManager.h"
#include "geTexture.h"
#include "../../Physics/core/vector2.h"
#if USE_ProgrammablePipeLine
#include "../hwShader/gxHWShader.h"
#include "../../Physics/core/matrix4x4f.h"
#endif
#include "../../Physics/core/transformx.h"

class Sprite : public transformx
{
public:
    
    enum ERENDER_FLAG
    {
        RENDER_NORMAL               = (1),
        RENDER_FORCE_NO_ALPHA       = (1<<1),
        RENDER_FORCE_NO_BLEND       = (1<<2),
        RENDER_FORCE_NO_MODULATION  = (1<<3),
        RENDER_FORCE_NO_TRANSFROM   = (1<<4),
        RENDER_V_FLIP               = (1<<5),
        RENDER_H_FLIP               = (1<<6)
    };
    
	enum EBLENDFLAG
	{
		BLEND_MULTIPLY,
		BLEND_ADDITIVE
	};

	Sprite();
	Sprite(Sprite& sprite);
	~Sprite();

	void loadTexture(CGETextureManager* textureManager, const char *pszFileName);
	
#if USE_ProgrammablePipeLine
    virtual void draw(gxHWShader* shader, const matrix4x4f& viewProjection) {}
#else
    virtual void draw(/*const matrix4x4f& parentTM, */vector2f* pos=NULL) {}
#endif

	void setClip(float clipX, float clipY, float width, float height);
	//void setClip(const bxRectf& rect)  {   setClip(rect.m_pos.x, rect.m_pos.y, rect.m_size.x, rect.m_size.y); }

	float getClipX()	{	return spriteClipX;	}
	float getClipY()	{	return spriteClipY;	}

	float getClipWidth()		{	return spriteClipWidth;	}
	float getClipHeight()	{	return spriteClipHeight;	}
	
	int getOrigWidth()	{	return (int)originalSize.x;	}
	int getOrigHeight()    {	return (int)originalSize.y;	}
	
    //bxRectf getClip()       {   return bxRectf(spriteClipX, spriteClipY, spriteClipWidth, spriteClipHeight);  }
	vector2f getOffset()		{	return spriteOffset;	}

	void setOffset(float x, float y);	// (0, 0) origin, (-1, 1) left top
    
    void debugDraw(vector2f* pos=NULL);
	
	virtual void transformationChanged(){}
    virtual void onSpritePreRender(){}
    virtual void onSpritePostRender(){}
    
    geTexture* getTexture()  {   return &textureObject; }
    
    void clearRenderFlag()          {   spriteRenderFlag=0;        }
    void setRenderFlag(int flag)    {  spriteRenderFlag|=flag;     }
    int getRenderFlag()             {   return spriteRenderFlag;   }
    bool isRenderFlag(int flags)    {   return ((spriteRenderFlag&flags)>0)?true:false;    }
	
    void setAlpha(float alpha)        {   this->alpha=alpha;     }
    float getAlpha()                  {   return alpha;    }
    
    void setRGB(float r, float g, float b)   {   spriteRGB[0]=r; spriteRGB[1]=g; spriteRGB[2]=b;    }
	float* getRGB()		{	return spriteRGB;	}
    void setVFlip();
    void setHFlip();
    
    void setAlphaThreshold(float threshold)   {   alphaThreshold=threshold;   }
    float getAlphaThreshokld()                {  return alphaThreshold;       }
    
    virtual void transformationChangedf()
    {    }

	void setBlendMode(EBLENDFLAG eFlag)		{	spriteBlendFlag=eFlag;	}
	EBLENDFLAG getBlendMode()				{	return spriteBlendFlag;	}

	void setPos(int x, int y)	{	setPositionx(ITOX(x), ITOX(y), 0);	}

    float* getOriginalVertexBuffer() { return spriteOrigVertexArray; }
	float* getVertexBuffer()	{	return spriteVertexArray;	}
	float* getTexCoordBuffer()	{	return spriteTextureCoordinateArray;	}

	void copySprite(Sprite& sprite);

    void setRotation(float r)   { rotation = r; }
    void setScale(float sx, float sy);
    void setTile(float tx, float ty);
protected:
	geTexture textureObject;
	vector2f originalSize;
	vector2f spriteOffset;
	float spriteVertexArray[8];
    float spriteOrigVertexArray[8];
	float spriteTextureCoordinateArray[8];
    float spriteClipX;
    float spriteClipY;
	float spriteClipWidth;
	float spriteClipHeight;
    int spriteRenderFlag;
    float alpha;
    float spriteRGB[3];
    float alphaThreshold;
    float rotation;
    vector2f scale;
    vector2f tile;
	EBLENDFLAG spriteBlendFlag;
};
