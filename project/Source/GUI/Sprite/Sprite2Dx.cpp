#include "Sprite2Dx.h"

Sprite2Dx::Sprite2Dx():
Sprite()
{
}

Sprite2Dx::Sprite2Dx(Sprite2Dx& sprite2dx):
Sprite(sprite2dx)
{
}

Sprite2Dx::~Sprite2Dx()
{
	
}

#if USE_ProgrammablePipeLine
void Sprite2Dx::draw(gxHWShader* shader, const matrix4x4f& viewProjection)
{
    matrix4x4f dummyTM;
    dummyTM.copyMatrix(*this);
    
    matrix4x4f rotationTM;
    rotationTM.setRotationMatrix(rotation, 0, 0, true);
    const float* u_mvp_m4x4= (viewProjection * (dummyTM*rotationTM)).getMatrix();
    shader->sendUniformTMfv("u_mvp_m4x4", u_mvp_m4x4, false, 4);
    
    shader->sendUniform4fv("u_color", this->spriteRGB);
	glVertexAttribPointer(shader->getAttribLoc("a_vertex_coord_v4"), 2, GL_FLOAT, GL_FALSE, 0, spriteVertexArray);
    glEnableVertexAttribArray(shader->getAttribLoc("a_vertex_coord_v4"));

    glVertexAttribPointer(shader->getAttribLoc("a_uv_coord0_v2"), 2, GL_FLOAT, GL_FALSE, 0, spriteTextureCoordinateArray);
	glEnableVertexAttribArray(shader->getAttribLoc("a_uv_coord0_v2"));
	
	//arun:texissue glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureObject.getTextureID());
//    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, (isRenderFlag(RENDER_FORCE_NO_MODULATION) && alpha==1.0f) ? GL_REPLACE : GL_MODULATE);

	if(textureObject.getTextureType()==geTexture::TEX_ALPHA && !isRenderFlag(RENDER_FORCE_NO_ALPHA))
	{
 		glEnable(GL_BLEND);
		if(spriteBlendFlag==BLEND_MULTIPLY)
		{
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else if(spriteBlendFlag==BLEND_ADDITIVE)
		{
			glBlendFunc(GL_ONE, GL_ONE);
		}
	}

	shader->sendUniform1i("u_diffuse_texture", 0);
    shader->sendUniform2f("u_uvtile", tile.x, tile.y);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	if (textureObject.getTextureType() != geTexture::TEX_UNDEFINED)
	{	
		glDisable(GL_BLEND);
		glDisableVertexAttribArray(shader->getAttribLoc("a_uv_coord0_v2"));
		//glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		//arun:texissue glDisable(GL_TEXTURE_2D);
	}
	
	glDisableVertexAttribArray(shader->getAttribLoc("a_vertex_coord_v4"));
}
#else

void Sprite2Dx::draw(/*const matrix4x4f& parentTM, */vector2f* pos)
{	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, spriteVertexArray);
    
	if (textureObject.getTextureType() != geTexture::TEX_UNDEFINED)
	{
		glActiveTexture(GL_TEXTURE0);
		//glClientActiveTexture(GL_TEXTURE0);	
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureObject.getTextureID());

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, (isRenderFlag(RENDER_FORCE_NO_MODULATION) && alpha == 1.0f) ? GL_REPLACE : GL_MODULATE);

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, spriteTextureCoordinateArray);
		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		if(textureObject.getTextureType()==geTexture::TEX_ALPHA && !isRenderFlag(RENDER_FORCE_NO_ALPHA))
		{
 			glEnable(GL_BLEND);
			if(spriteBlendFlag==BLEND_MULTIPLY)
			{
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			else if(spriteBlendFlag==BLEND_ADDITIVE)
			{
				//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				glBlendFunc(GL_ONE, GL_ONE);
			}
		}
        
        glMatrixMode(GL_TEXTURE);
        glScalef(tile.x, tile.y, 1.0f);
        glMatrixMode(GL_MODELVIEW);
	}

    if(!isRenderFlag(RENDER_FORCE_NO_ALPHA | RENDER_FORCE_NO_BLEND))
    {
        glColor4f(spriteRGB[0], spriteRGB[1], spriteRGB[2], alpha);
    }
    
	glPushMatrix();
    if(!isRenderFlag(RENDER_FORCE_NO_TRANSFROM))
    {
		if(pos)
			glTranslatef(pos->x, pos->y, 0.0f);
		else
			glTranslatef(XTOF(m[12]), XTOF(m[13]), XTOF(m[14]));
    }
    
    glRotatef(rotation, 0, 0, 1);
    
    onSpritePreRender();

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    onSpritePostRender();
    glPopMatrix();
	
	if (textureObject.getTextureType() != geTexture::TEX_UNDEFINED)
	{
        glMatrixMode(GL_TEXTURE);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        
		glDisable(GL_BLEND);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		//glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
	}
	
	glDisableClientState(GL_VERTEX_ARRAY);
}
#endif

void Sprite2Dx::drawVerextBufferOnly(vector2f* pos)
{
#if !USE_ProgrammablePipeLine
    glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, spriteVertexArray);

    glPushMatrix();
	if(pos)
		glTranslatef(pos->x, pos->y, 0.0f);
	else
		glTranslatef(m[12], m[13], m[14]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glPopMatrix();
    
    glDisableClientState(GL_VERTEX_ARRAY);
#endif
}

