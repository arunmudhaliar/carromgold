#include "gxRenderer.h"

gxRenderer::gxRenderer()
{
	projectionMatrix=NULL;
	viewMatrix=NULL;
	viewProjectionMatrix=NULL;
//    texturePacket1x1=NULL;
	viewPortRectangle.set(0.0f, 0.0f, 1.0f, 1.0f);
	noOfTrianglesRendered=0;
	noOfDrawCalls=0;
//    setRenderPassType(RENDER_NORMAL);
}

gxRenderer::~gxRenderer()
{
}

void gxRenderer::setViewPort(float x, float y, float cx, float cy)
{
	if(cx<=1.0f)
		cx=1.0f;
	if(cy<=1.0f)
		cy=1.0f;

	viewPortRectangle.set(x, y, cx, cy);
	glViewport((int)x, (int)y, (int)cx, (int)cy);
}

void gxRenderer::setOrthoProjectionMatrix(matrix4x4f* matrix) {
    orthogonalProjectionMatrix = *matrix;
}

void gxRenderer::setProjectionMatrixToGL(matrix4x4f* matrix)
{
	projectionMatrix=matrix;
#if defined (USE_ProgrammablePipeLine)
#else
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(matrix->getMatrix());
    glMatrixMode(GL_MODELVIEW);
#endif
}

void gxRenderer::setViewMatrixToGL(matrix4x4f* matrix)
{
	viewMatrix=matrix;
#if defined (USE_ProgrammablePipeLine)
#else
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(matrix->getMatrix());
#endif
}

void gxRenderer::setViewProjectionMatrix(matrix4x4f* matrix)
{
	viewProjectionMatrix=matrix;
}
