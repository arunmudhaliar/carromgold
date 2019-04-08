#pragma once

#include "Sprite.h"

class Sprite2Dx : public Sprite
{
public:	
	Sprite2Dx();
	Sprite2Dx(Sprite2Dx& sprite2dx);

	~Sprite2Dx();
	
#if USE_ProgrammablePipeLine
    virtual void draw(gxHWShader* shader, const matrix4x4f& viewProjection);
#else
    virtual void draw(/*const matrix4x4f& parentTM, */vector2f* pos=NULL);
#endif
	
//#ifndef GEAR2D
//    virtual void draw(gxHWShader* shader);
//#endif

    virtual void onSpritePreRender(){};
    virtual void onSpritePostRender(){};

    void drawVerextBufferOnly(vector2f* pos=NULL);
};

