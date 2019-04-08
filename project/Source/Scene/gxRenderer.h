#pragma once

#include "../Physics/core/matrix4x4f.h"
#include "../Physics/core/rect.h"
#if GEAR_APPLE
    #include <OpenGL/gl.h>
#elif GEAR_ANDROID
    #include <GLES2/gl2.h>
#else
    #include <gl.h>
#endif
//#include "../core/TextureManager.h"
//#include "stFog.h"

class gxRenderer
{
public:
//    enum RENDER_PASS_TYPE
//    {
//        RENDER_NORMAL,
//        RENDER_LIGHTING_ONLY,
//        RENDER_SHADOWMAP
//    };

	gxRenderer();
	~gxRenderer();

	void setViewPort(float x, float y, float cx, float cy);

	void setProjectionMatrixToGL(matrix4x4f* matrix);
	void setViewMatrixToGL(matrix4x4f* matrix);
	void setViewProjectionMatrix(matrix4x4f* matrix);

	const vector2f& getViewPortSz()			{	return viewPortRectangle.m_size;			}
	const gxRectf& getViewPortRect()		{	return viewPortRectangle;					}

	matrix4x4f* getProjectionMatrix()		{	return projectionMatrix;			}
	matrix4x4f* getViewMatrix()				{	return viewMatrix;				}
	matrix4x4f* getViewProjectionMatrix()	{	return viewProjectionMatrix;		}
	matrix4x4f* getOrthoProjectionMatrix()	{	return &orthogonalProjectionMatrix;	}

//    void setGEARTexture1x1(stTexturePacket* tex)    {    texturePacket1x1=tex;    }
//    stTexturePacket* getGEARTexture1x1()            {    return texturePacket1x1;        }

//    void setRenderPassType(RENDER_PASS_TYPE eType)    {    renderPassType=eType;    }
//    RENDER_PASS_TYPE getRenderPassType()            {    return renderPassType;    }

	void setMainCameraEye(vector3f eye)		{	mainCameraEyePosition=eye;	}
	vector3f getMainCameraEye()				{	return mainCameraEyePosition;	}

//    stFog* getFog()    {    return &fogStruct; }

    unsigned int noOfTrianglesRendered;
    unsigned int noOfDrawCalls;

private:

	gxRectf	viewPortRectangle;
	matrix4x4f* projectionMatrix;			//must not delete this pointer
	matrix4x4f* viewMatrix;				//must not delete this pointer
	matrix4x4f* viewProjectionMatrix;     //must not delete this pointer
    matrix4x4f  orthogonalProjectionMatrix;

//    stTexturePacket* texturePacket1x1;        //must not delete this pointer
//    RENDER_PASS_TYPE renderPassType;
	vector3f mainCameraEyePosition;

//    stFog fogStruct;
};
