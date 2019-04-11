//
//  geFontManager.cpp
//  waves2
//
//  Created by arun on 03/05/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "geFontManager.h"
#include "../Physics/core/types.h"
#include "OSUtils.h"
#include <string>

#if defined (USE_ProgrammablePipeLine)
geFont::geFont(gxHWShader* pFontShaderPtr, gxRenderer* m_pRendererPtr)
{
    m_pFontShaderPtr=pFontShaderPtr;
	this->m_pRendererPtr=m_pRendererPtr;
#else
    geFont::geFont()
    {
#endif
    m_iBitmapWidth=m_iBitmapHeight=m_iBaseChar=m_nChars=0;
    m_pszU=m_pszV=m_pszU_width=m_pszU_height=NULL;
    m_pszCharW=m_pszCharH=NULL;
    m_pszCharD=NULL;
    m_pszCharOffsetX=m_pszCharOffsetY=NULL;
    m_iTexID=0;
    m_iSpaceWidth=0;
    m_iLineHeight=0;
    m_bDeleteGLTexture=true;
    setRGBA(1.0f, 1.0f, 1.0f, 1.0f);
    setYOffset(0.0f);
}

geFont::~geFont()
{
    reset();
}

void geFont::reset()
{
    if(m_iTexID>0 && m_bDeleteGLTexture)
    {
        glDeleteTextures(1, &m_iTexID);
        m_iTexID=0;
    }
    
    GX_DELETE_ARY(m_pszCharOffsetX);
    GX_DELETE_ARY(m_pszCharOffsetY);
    GX_DELETE_ARY(m_pszCharW);
    GX_DELETE_ARY(m_pszCharH);
    GX_DELETE_ARY(m_pszCharD);
    
    GX_DELETE_ARY(m_pszU);
    GX_DELETE_ARY(m_pszV);
    GX_DELETE_ARY(m_pszU_width);
    GX_DELETE_ARY(m_pszU_height);
#if defined (USE_ProgrammablePipeLine)
    m_pFontShaderPtr=NULL;
#endif
    
#if LOG_DEBUG_ENGINE
    DEBUG_PRINT("geFont Destroyed...");
#endif 
}

bool geFont::load(gxFile& file)
{
    file.Read(m_iBitmapWidth);
    file.Read(m_iBitmapHeight);
    file.Read(m_nChars);
    file.Read(m_iBaseChar);
    file.Read(m_iSpaceWidth);    
    
#if LOG_DEBUG_ENGINE
    DEBUG_PRINT("font width=%d, height=%d, buffersize=%d", m_iBitmapWidth, m_iBitmapHeight, m_iBitmapHeight*m_iBitmapWidth*2);
#endif 
    if(m_nChars==0) return false;
    
    unsigned char* buffer=new unsigned char[m_iBitmapHeight*m_iBitmapWidth*2];
    
    m_pszU=new float[m_nChars];
    m_pszV=new float[m_nChars];
    m_pszU_width=new float[m_nChars];
    m_pszU_height=new float[m_nChars];
    m_pszCharW=new unsigned short[m_nChars];
    m_pszCharH=new unsigned short[m_nChars];
    m_pszCharD=new short[m_nChars];
    m_pszCharOffsetX=new short[m_nChars];
    m_pszCharOffsetY=new short[m_nChars];
    
    for(int x=0;x<m_nChars;x++)
    {
        file.Read(m_pszCharOffsetX[x]);
        file.Read(m_pszCharOffsetY[x]);

        file.Read(m_pszCharW[x]);
        file.Read(m_pszCharH[x]);
        file.Read(m_pszCharD[x]);
        file.Read(m_pszU[x]);
        file.Read(m_pszV[x]);
        file.Read(m_pszU_width[x]);
        file.Read(m_pszU_height[x]);
    }
    
    file.Read(m_iLineHeight);
    file.ReadBuffer(buffer, m_iBitmapHeight*m_iBitmapWidth*2);
    m_iTexID=loadBuffer(buffer, true, m_iBitmapWidth, m_iBitmapHeight, 16);
    GX_DELETE_ARY(buffer);
    
    return true;
}

unsigned int geFont::loadBuffer(unsigned char* buffer, bool bAlpha, unsigned int width, unsigned int height, unsigned int bpp)
{
    unsigned int texID=0;
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
        
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    switch (bpp) {
        case 16:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, buffer);
            break;
        case 32:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
            break;
        default:
            break;
    }
        
        
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
        
#if LOG_DEBUG_ENGINE
    DEBUG_PRINT("texture loaded from buffer - w=%d, h=%d", width, height);
#endif 	
        
        
    return texID;
}

void geFont::draw()
{
    const int vertLst[8] =
	{
		m_iBitmapWidth*65536,	0,
		0,                      0,
		m_iBitmapWidth*65536,	m_iBitmapHeight*65536,
		0,                      m_iBitmapHeight*65536
	};
    
    const int tex[8] =
	{
		65536,  0,
		0,      0,
		65536,  65536,
		0,      65536,
	};

#if defined (USE_ProgrammablePipeLine)
#else
#if ARUN_COMMENTED
    glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FIXED, 0, vertLst);

    glActiveTexture(GL_TEXTURE0);
    glClientActiveTexture(GL_TEXTURE0);
    glTexCoordPointer(2, GL_FIXED, 0, tex);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_iTexID);
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    //arun_commenting_alphatest glEnable(GL_ALPHA_TEST);
    //arun_commenting_alphatest glAlphaFuncx(GL_GREATER, 6553); //0.1f
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisable(GL_BLEND);
    //arun_commenting_alphatest glDisable(GL_ALPHA_TEST);

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_VERTEX_ARRAY);
#endif
#endif
}

float geFont::calculateStringWidthInPixelTillNewLine(const char* str, int nChar, int iCurIndex)
{
    float kerning=GE_DEBUG_KERNING;
    float width=0.0f;
    int diff=nChar-iCurIndex;
    int cntr=0;
    while(diff--)
    {
        int actual_char=(unsigned char)str[iCurIndex+cntr];
        int ch=(unsigned char)str[iCurIndex+cntr]-m_iBaseChar;

        if(actual_char=='\n')
        {
            break;
        }
        else if(actual_char=='\t')
        {
            width+=(m_pszCharW[32-m_iBaseChar]*5);
            continue;
        }
        if(ch<0 || ch>=m_nChars)
        {
            cntr++;
            continue;
        }
        width+=((float)m_pszCharD[ch]+kerning);
        cntr++;
    }
    
    return width;
}

float geFont::getCharWidth(char charValue)
{
	float kerning=GE_DEBUG_KERNING;
	int ch=(unsigned char)charValue-m_iBaseChar;
	return ((float)m_pszCharD[ch]+kerning);
}

#define CHAR_DELAY  0.05f
int geFont::drawString(const char* str, int x, int y, int width_limit, bool bCentered, bool bShadowed, float elapsetime)
{
    int retVal=1;
    int actual_len=(int)strlen(str);
    int len=(elapsetime==0.0f)?actual_len:(int)(elapsetime/CHAR_DELAY);
    if(len>actual_len){len=actual_len;}
    else
    { retVal=0; }
    
    int cntr=0;
    
    float raster_pos_x=0.0f;
    float raster_pos_y=0.0f;
    float kerning=GE_DEBUG_KERNING;

    float width=0.0f;
    if(bCentered)
    {
        width=calculateStringWidthInPixelTillNewLine(str, len, 0);
        raster_pos_x=-width*0.5f;
    }
    
    for(int xx=0;xx<len;xx++)
    {
        int actual_char=(unsigned char)str[xx];
        int ch=(unsigned char)str[xx]-m_iBaseChar;
        
        if(actual_char=='\n')
        {
            raster_pos_y+=(float)m_iLineHeight;
            raster_pos_x=0.0f;
            if(bCentered)
            {
                width=calculateStringWidthInPixelTillNewLine(str, len, xx+1);
                raster_pos_x=-width*0.5f;
            }
            continue;
        }
        else if(actual_char=='\t')
        {
            raster_pos_x+=(m_pszCharW[32-m_iBaseChar]*5);
            continue;
        }
        
        if(ch<0 || ch>=m_nChars)
        {
            continue;
        }

        int offx=m_pszCharOffsetX[ch];
        int offy=m_pszCharOffsetY[ch];
        
        float left=raster_pos_x+offx;
        float right=raster_pos_x+m_pszCharW[ch]+offx;
        float top=raster_pos_y+offy;
        float bottom=raster_pos_y+m_pszCharH[ch]+offy;
        
        m_cszVertCoordList[cntr*12+0]=(short)right;     m_cszVertCoordList[cntr*12+1]=(short)top;
        m_cszVertCoordList[cntr*12+2]=(short)left;      m_cszVertCoordList[cntr*12+3]=(short)top;
        m_cszVertCoordList[cntr*12+4]=(short)right;     m_cszVertCoordList[cntr*12+5]=(short)bottom;
        m_cszVertCoordList[cntr*12+6]=(short)left;      m_cszVertCoordList[cntr*12+7]=(short)bottom;
        m_cszVertCoordList[cntr*12+8]=(short)right;     m_cszVertCoordList[cntr*12+9]=(short)bottom;
        m_cszVertCoordList[cntr*12+10]=(short)left;     m_cszVertCoordList[cntr*12+11]=(short)top;


        m_cszTexCoordList[cntr*12+0]=(m_pszU[ch]+m_pszU_width[ch]);   m_cszTexCoordList[cntr*12+1]=m_pszV[ch];
        m_cszTexCoordList[cntr*12+2]=m_pszU[ch];                      m_cszTexCoordList[cntr*12+3]=m_pszV[ch];
        m_cszTexCoordList[cntr*12+4]=(m_pszU[ch]+m_pszU_width[ch]);   m_cszTexCoordList[cntr*12+5]=(m_pszV[ch]+m_pszU_height[ch]);
        m_cszTexCoordList[cntr*12+6]=m_pszU[ch];                      m_cszTexCoordList[cntr*12+7]=(m_pszV[ch]+m_pszU_height[ch]);
        m_cszTexCoordList[cntr*12+8]=(m_pszU[ch]+m_pszU_width[ch]);   m_cszTexCoordList[cntr*12+9]=(m_pszV[ch]+m_pszU_height[ch]);
        m_cszTexCoordList[cntr*12+10]=m_pszU[ch];                     m_cszTexCoordList[cntr*12+11]=m_pszV[ch];

        raster_pos_x+=((float)m_pszCharD[ch]+kerning);
		if(raster_pos_x>(float)width_limit)
		{
			retVal=9;	//window bound
			break;
		}
        cntr++;
    }
    
#if defined (USE_ProgrammablePipeLine)
    m_pFontShaderPtr->enableProgram();
    //m_pFontShaderPtr->resetAllFlags();
    
    glVertexAttribPointer(m_pFontShaderPtr->getAttribLoc("a_vertex_coord_v4"), 2, GL_SHORT, GL_FALSE, 0, m_cszVertCoordList);
    glEnableVertexAttribArray(m_pFontShaderPtr->getAttribLoc("a_vertex_coord_v4"));
    
    glVertexAttribPointer(m_pFontShaderPtr->getAttribLoc("a_uv_coord0_v2"), 2, GL_FLOAT, GL_FALSE, 0, m_cszTexCoordList);
    glEnableVertexAttribArray(m_pFontShaderPtr->getAttribLoc("a_uv_coord0_v2"));
    
    //glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_iTexID);	
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_ONE, GL_ONE);
    
    matrix4x4f transformTM;
    
    if(bShadowed)
    {
//        transformTM.setRotationMatrix(180, true, 0, 0);
        transformTM.setPosition(x+1, y+1+m_fYOffset, 0);
        transformTM= *m_pRendererPtr->getOrthoProjectionMatrix() * transformTM;
        const float* u_mvp_m4x4_shadow=transformTM.getMatrix();
        m_pFontShaderPtr->sendUniformTMfv("u_mvp_m4x4", u_mvp_m4x4_shadow, false, 4);
        float shadow_color[]={0.0f, 0.0f, 0.0f, 0.7f};
        m_pFontShaderPtr->sendUniform4fv("u_color_v4", shadow_color);
        glDrawArrays(GL_TRIANGLES, 0, cntr*6);
        
        transformTM.identity(); //reset the matrix
    }
    
//    transformTM.setRotationMatrix(180, true, 0, 0);
    transformTM.setPosition(x, y+m_fYOffset, 0);
    transformTM= *m_pRendererPtr->getOrthoProjectionMatrix() * transformTM;
    const float* u_mvp_m4x4=transformTM.getMatrix();
    m_pFontShaderPtr->sendUniformTMfv("u_mvp_m4x4", u_mvp_m4x4, false, 4);
    m_pFontShaderPtr->sendUniform4fv("u_color_v4", m_cszRGBA);
    glDrawArrays(GL_TRIANGLES, 0, cntr*6);
    
    glDisable(GL_BLEND);
    glDisableVertexAttribArray(m_pFontShaderPtr->getAttribLoc("a_uv_coord0_v2"));
    //glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisableVertexAttribArray(m_pFontShaderPtr->getAttribLoc("a_vertex_coord_v4"));
    
    m_pFontShaderPtr->disableProgram();
#else
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_SHORT, 0, m_cszVertCoordList);
    
    //glActiveTexture(GL_TEXTURE0);
    //glClientActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_iTexID);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, m_cszTexCoordList);
    
    //arun_commenting_alphatest glEnable(GL_ALPHA_TEST);
    //arun_commenting_alphatest glAlphaFuncx(GL_GREATER, 6553); //0.1f
	glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_ONE, GL_ONE);

    glPushMatrix();
    if(bShadowed)
    {   
        glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
        glTranslatef(x+1, y+1+m_fYOffset, 0);
        glDrawArrays(GL_TRIANGLES, 0, cntr*6);
        glTranslatef(-1, -1, 0);
    }
    else
    {
        glTranslatef(x, y+m_fYOffset, 0);
    }
    glColor4f(m_cszRGBA[0], m_cszRGBA[1], m_cszRGBA[2], m_cszRGBA[3]);
    glDrawArrays(GL_TRIANGLES, 0, cntr*6);
    glPopMatrix();
    
    glDisable(GL_BLEND);
    //arun_commenting_alphatest glDisable(GL_ALPHA_TEST);
    
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_VERTEX_ARRAY);
#endif
    
    return retVal;
}

void geFont::drawOnBuffer(const char* str, int cx, int cy, unsigned int& nChar, short* vertexCoordBuffer, float* textureCoordBuffer, unsigned int& nLines, unsigned short* linebuffer, ALIGNMENT alignment) const
{
    switch (alignment)
    {
        case ALIGN_LEFT:
            drawOnBuffer_left(str, cx, cy, nChar, vertexCoordBuffer, textureCoordBuffer, nLines, linebuffer);
            break;
        case ALIGN_RIGHT:
            drawOnBuffer_left(str, cx, cy, nChar, vertexCoordBuffer, textureCoordBuffer, nLines, linebuffer);
            break;
        case ALIGN_CENTER:
            drawOnBuffer_center(str, cx, cy, nChar, vertexCoordBuffer, textureCoordBuffer, nLines, linebuffer);
            break;
        case ALIGN_JUSTIFY:
            drawOnBuffer_justify(str, cx, cy, nChar, vertexCoordBuffer, textureCoordBuffer, nLines, linebuffer);
            break;
    }
}

void geFont::drawOnBuffer_justify(const char* str, int cx, int cy, unsigned int& nChar, short* vertexCoordBuffer, float* textureCoordBuffer, unsigned int& nLines, unsigned short* linebuffer) const
{
    nLines=0;
    int len=nChar;
    int cntr=0;
    
    float raster_pos_x=0.0f;
    float raster_pos_y=0.0f;
    float kerning=GE_DEBUG_KERNING;
    bool bLineStarted=false;
    float extraspace=0.0f;
    float extra_pixels_added_to_space=0.0f;
    int nWords=0;
    if(calculateExtraSpaceOnRightSide(str, cx, nChar, 0, extraspace, nWords))
    {
        if(nWords)
        {
            extra_pixels_added_to_space=extraspace/nWords;
        }
    }
    else
    {
        extraspace=0.0f;
    }
    
    unsigned int nCharsOnLine=0;
    bool bCalculateNextLine=false;
    for(int x=0;x<len;x++)
    {
        int actual_char=(unsigned char)str[x];
        int ch=actual_char-m_iBaseChar;
        
        if(actual_char=='\n')
        {
            if(bCalculateNextLine)
            {
                if(nLines==0)
                    linebuffer[nLines]=nCharsOnLine;
                else
                    linebuffer[nLines]=linebuffer[nLines-1]+nCharsOnLine;
                nCharsOnLine=0;
                nLines++;
            }
            
            bCalculateNextLine=true;
            raster_pos_y+=(float)m_iLineHeight;

            continue;
        }
        else if(actual_char=='\t')
        {
            raster_pos_x+=(m_pszCharW[32-m_iBaseChar]*5);
            continue;
        }
        
        if(bCalculateNextLine)
        {
            if(calculateExtraSpaceOnRightSide(str, cx, nChar, x, extraspace, nWords))
            {
                if(nWords)
                {
                    extra_pixels_added_to_space=extraspace/nWords;
                }
            }
            else
            {
                extra_pixels_added_to_space=0.0f;
                extraspace=0.0f;
            }
            raster_pos_x=0.0f;
            if(nLines==0)
                linebuffer[nLines]=nCharsOnLine;
            else
                linebuffer[nLines]=linebuffer[nLines-1]+nCharsOnLine;
            bLineStarted=false;
            nCharsOnLine=0;
            nLines++;
            bCalculateNextLine=false;
        }
        
        if(ch<0 || ch>=m_nChars)
        {
            continue;
        }
        
        if(raster_pos_x+(float)m_pszCharD[ch]+kerning>cx)
        {
            bCalculateNextLine=true;
            raster_pos_y+=(float)m_iLineHeight;
            x--;
            continue;
        }
        
        if(!bLineStarted)
        {
            if(actual_char==32)
                continue;
            else
                bLineStarted=true;
        }
        
        int offx=m_pszCharOffsetX[ch];
        int offy=m_pszCharOffsetY[ch];
        
        float left=raster_pos_x+offx;
        float right=raster_pos_x+m_pszCharW[ch]+offx;
        float top=raster_pos_y+offy;
        float bottom=raster_pos_y+m_pszCharH[ch]+offy;
        
        vertexCoordBuffer[cntr*12+0]=(short)right;     vertexCoordBuffer[cntr*12+1]=(short)top;
        vertexCoordBuffer[cntr*12+2]=(short)left;      vertexCoordBuffer[cntr*12+3]=(short)top;
        vertexCoordBuffer[cntr*12+4]=(short)right;     vertexCoordBuffer[cntr*12+5]=(short)bottom;
        vertexCoordBuffer[cntr*12+6]=(short)left;      vertexCoordBuffer[cntr*12+7]=(short)bottom;
        vertexCoordBuffer[cntr*12+8]=(short)right;     vertexCoordBuffer[cntr*12+9]=(short)bottom;
        vertexCoordBuffer[cntr*12+10]=(short)left;     vertexCoordBuffer[cntr*12+11]=(short)top;
        raster_pos_x+=((float)m_pszCharD[ch]+kerning);
        if(actual_char==32)
            raster_pos_x+=extra_pixels_added_to_space;
        
        textureCoordBuffer[cntr*12+0]=(m_pszU[ch]+m_pszU_width[ch]);   textureCoordBuffer[cntr*12+1]=m_pszV[ch];
        textureCoordBuffer[cntr*12+2]=m_pszU[ch];                      textureCoordBuffer[cntr*12+3]=m_pszV[ch];
        textureCoordBuffer[cntr*12+4]=(m_pszU[ch]+m_pszU_width[ch]);   textureCoordBuffer[cntr*12+5]=(m_pszV[ch]+m_pszU_height[ch]);
        textureCoordBuffer[cntr*12+6]=m_pszU[ch];                      textureCoordBuffer[cntr*12+7]=(m_pszV[ch]+m_pszU_height[ch]);
        textureCoordBuffer[cntr*12+8]=(m_pszU[ch]+m_pszU_width[ch]);   textureCoordBuffer[cntr*12+9]=(m_pszV[ch]+m_pszU_height[ch]);
        textureCoordBuffer[cntr*12+10]=m_pszU[ch];                     textureCoordBuffer[cntr*12+11]=m_pszV[ch];
        
        cntr++;
        nCharsOnLine++;
    }
    nChar=cntr;
    if(nLines==0)
        linebuffer[nLines]=nChar;
    else
        linebuffer[nLines]=linebuffer[nLines-1]+nCharsOnLine;
    nLines++;
}

bool geFont::calculateExtraSpaceOnRightSide(const char* str, int cx, int nChar, int iCurIndex, float& extraspace, int& nWords) const
{   
    nWords=0;
    float raster_pos_x=0.0f;
    float kerning=GE_DEBUG_KERNING;
    float last_space_charXPos=0.0f;
    bool bLineStarted=false;
    bool bCarriageReturnEncountered=false;
    bool bNeglectLastWord=true;
    bool bEndReached=false;
    
    int x=0;
    for(x=iCurIndex;x<nChar;x++)
    {
        int actual_char=(unsigned char)str[x];
        int ch=actual_char-m_iBaseChar;
//        const char* current_char=&str[x];
        
        if(!bLineStarted)
        {
            if(actual_char==32)
                continue;
            else
                bLineStarted=true;
        }
        
        if(actual_char==32) //space
        {
            nWords++;
            last_space_charXPos=raster_pos_x;
        }
        else if(actual_char=='\n')
        {
            raster_pos_x=0.0f;
            bCarriageReturnEncountered=true;
            break;  //found eol
        }
        else if(actual_char=='\t')
        {
            raster_pos_x+=(m_pszCharW[32-m_iBaseChar]*5);
            continue;
        }
        if(ch<0 || ch>=m_nChars)
        {
            continue;
        }

        if(raster_pos_x+(float)m_pszCharD[ch]+kerning>cx)
        {
            if(actual_char==32 || actual_char=='\n' || actual_char=='\t')
            {
                bNeglectLastWord=false;
            }
            raster_pos_x=0.0f;
            break; //found eol
        }
        
        raster_pos_x+=((float)m_pszCharD[ch]+kerning);
    }
    
    if(x==nChar)
        bEndReached=true;
    
    extraspace=cx-last_space_charXPos;
    
    if(bNeglectLastWord)
        nWords--;
    
    return (bLineStarted && !bCarriageReturnEncountered && bNeglectLastWord && !bEndReached);
}

void geFont::drawOnBuffer_center(const char* str, int cx, int cy, unsigned int& nChar, short* vertexCoordBuffer, float* textureCoordBuffer, unsigned int& nLines, unsigned short* linebuffer) const
{
    nLines=0;
    int len=nChar;
    int cntr=0;
    
    float raster_pos_x=0.0f;
    float raster_pos_y=0.0f;
    float kerning=GE_DEBUG_KERNING;
    bool bLineStarted=false;
    float extraspace=0.0f;
    int nWords=0;
    if(calculateExtraSpaceOnBothSide(str, cx, nChar, 0, extraspace, nWords))
    {
        if(nWords)
        {
            raster_pos_x=(extraspace/2);
        }
    }
    
    unsigned int nCharsOnLine=0;
    bool bCalculateNextLine=false;
    for(int x=0;x<len;x++)
    {
        int actual_char=(unsigned char)str[x];
        int ch=actual_char-m_iBaseChar;
        //const char* temp=&str[x];
        
        if( actual_char=='\n')
        {
            raster_pos_y+=(float)m_iLineHeight;
            bCalculateNextLine=true;
            continue;
        }
        
        if(bCalculateNextLine)
        {
            raster_pos_x=0.0f;
            if(calculateExtraSpaceOnBothSide(str, cx, nChar, x, extraspace, nWords))
            {
                if(nWords)
                {
                    raster_pos_x=(extraspace/2);
                }
            }

            if(nLines==0)
                linebuffer[nLines]=nCharsOnLine;
            else
                linebuffer[nLines]=linebuffer[nLines-1]+nCharsOnLine;

            bLineStarted=false;
            nCharsOnLine=0;
            nLines++;
            bCalculateNextLine=false;
        }

        if(canNeglectNextWord(str, cx, nChar, x, raster_pos_x))
        {
            bCalculateNextLine=true;
            raster_pos_y+=(float)m_iLineHeight;
            continue;
        }
        

        if(actual_char=='\t')
        {
            raster_pos_x+=(m_pszCharW[32-m_iBaseChar]*5);
            continue;
        }
        if(ch<0 || ch>=m_nChars)
        {
            continue;
        }
        
        if(raster_pos_x+(float)m_pszCharD[ch]+kerning>cx)
        {
            bCalculateNextLine=true;
        }
        
        if(!bLineStarted)
        {
            if(actual_char==32)
                continue;
            else
                bLineStarted=true;
        }
        
        int offx=m_pszCharOffsetX[ch];
        int offy=m_pszCharOffsetY[ch];
        
        float left=raster_pos_x+offx;
        float right=raster_pos_x+m_pszCharW[ch]+offx;
        float top=raster_pos_y+offy;
        float bottom=raster_pos_y+m_pszCharH[ch]+offy;
        
        vertexCoordBuffer[cntr*12+0]=(short)right;     vertexCoordBuffer[cntr*12+1]=(short)top;
        vertexCoordBuffer[cntr*12+2]=(short)left;      vertexCoordBuffer[cntr*12+3]=(short)top;
        vertexCoordBuffer[cntr*12+4]=(short)right;     vertexCoordBuffer[cntr*12+5]=(short)bottom;
        vertexCoordBuffer[cntr*12+6]=(short)left;      vertexCoordBuffer[cntr*12+7]=(short)bottom;
        vertexCoordBuffer[cntr*12+8]=(short)right;     vertexCoordBuffer[cntr*12+9]=(short)bottom;
        vertexCoordBuffer[cntr*12+10]=(short)left;     vertexCoordBuffer[cntr*12+11]=(short)top;
        raster_pos_x+=((float)m_pszCharD[ch]+kerning);
        
        textureCoordBuffer[cntr*12+0]=(m_pszU[ch]+m_pszU_width[ch]);   textureCoordBuffer[cntr*12+1]=m_pszV[ch];
        textureCoordBuffer[cntr*12+2]=m_pszU[ch];                      textureCoordBuffer[cntr*12+3]=m_pszV[ch];
        textureCoordBuffer[cntr*12+4]=(m_pszU[ch]+m_pszU_width[ch]);   textureCoordBuffer[cntr*12+5]=(m_pszV[ch]+m_pszU_height[ch]);
        textureCoordBuffer[cntr*12+6]=m_pszU[ch];                      textureCoordBuffer[cntr*12+7]=(m_pszV[ch]+m_pszU_height[ch]);
        textureCoordBuffer[cntr*12+8]=(m_pszU[ch]+m_pszU_width[ch]);   textureCoordBuffer[cntr*12+9]=(m_pszV[ch]+m_pszU_height[ch]);
        textureCoordBuffer[cntr*12+10]=m_pszU[ch];                     textureCoordBuffer[cntr*12+11]=m_pszV[ch];
        
        cntr++;
        nCharsOnLine++;
    }
    nChar=cntr;
    nLines++;
}

bool geFont::calculateExtraSpaceOnBothSide(const char* str, int cx, int nChar, int iCurIndex, float& extraspace, int& nWords) const
{
    nWords=0;
    float raster_pos_x=0.0f;
    float kerning=GE_DEBUG_KERNING;
    float last_space_charXPos=0.0f;
    bool bLineStarted=false;
    bool bCarriageReturnEncountered=false;
    bool bNeglectLastWord=true;
    bool bEndReached=false;
    
    int x=0;
    for(x=iCurIndex;x<nChar;x++)
    {
        int actual_char=(unsigned char)str[x];
        int ch=actual_char-m_iBaseChar;
        //        const char* current_char=&str[x];
        
        if(!bLineStarted)
        {
            if(actual_char==32)
            {
                if(raster_pos_x+(float)m_iSpaceWidth+kerning>cx)
                {
                    raster_pos_x=0.0f;
                    return false;
                }
                else
                    raster_pos_x+=(m_iSpaceWidth+kerning);
                continue;
            }
            else
                bLineStarted=true;
        }
        
        if(actual_char==32) //space
        {
            nWords++;
            last_space_charXPos=raster_pos_x;
        }
        else if(actual_char=='\n')
        {
            if(bLineStarted)
                nWords++;
            last_space_charXPos=raster_pos_x;   //chk
            raster_pos_x=0.0f;
            bCarriageReturnEncountered=true;
            bNeglectLastWord=false;
            break;  //found eol
        }
        else if(actual_char=='\t')
        {
            raster_pos_x+=(m_pszCharW[32-m_iBaseChar]*5);
            continue;
        }
        if(ch<0 || ch>=m_nChars)
        {
            continue;
        }
        
        if(raster_pos_x+(float)m_pszCharD[ch]+kerning>cx)
        {
            if(actual_char==32 || actual_char=='\n' || actual_char=='\t')
            {
                bNeglectLastWord=false;
            }
            raster_pos_x=0.0f;
            break; //found eol
        }
        
        raster_pos_x+=((float)m_pszCharD[ch]+kerning);
    }
    
    if(x==nChar)
        bEndReached=true;
    
    extraspace=cx-last_space_charXPos;
    
    if(bNeglectLastWord)
        nWords--;
    
    return (bLineStarted);
}

void geFont::drawOnBuffer_left(const char* str, int cx, int cy, unsigned int& nChar, short* vertexCoordBuffer, float* textureCoordBuffer, unsigned int& nLines, unsigned short* linebuffer) const
{
    nLines=0;
    int len=nChar;
    int cntr=0;
    
    float raster_pos_x=0.0f;
    float raster_pos_y=0.0f;
    float kerning=GE_DEBUG_KERNING;
    bool bLineStarted=false;
//    float extraspace=0.0f;
//    float extra_pixels_added_to_space=0.0f;
//    int nWords=0;
//    if(calculateExtraSpaceOnLine(str, cx, nChar, 0, extraspace, nWords))
//    {
//        if(nWords)
//        {
//            extra_pixels_added_to_space=extraspace/nWords;
//        }
//    }
//    else
//    {
//        extraspace=0.0f;
//    }
    
    unsigned int nCharsOnLine=0;
    for(int x=0;x<len;x++)
    {
        int actual_char=(unsigned char)str[x];
        int ch=actual_char-m_iBaseChar;
        
        if(actual_char==32 && bLineStarted)
        {
            if(canNeglectNextWord(str, cx, nChar, x, raster_pos_x))
            {
                raster_pos_x=0.0f;
                raster_pos_y+=(float)m_iLineHeight;
                if(nLines==0)
                    linebuffer[nLines]=nCharsOnLine;
                else
                    linebuffer[nLines]=linebuffer[nLines-1]+nCharsOnLine;

                bLineStarted=false;
                nCharsOnLine=0;
                nLines++;
                continue;
            }
        }
        else if(actual_char=='\n')
        {
            raster_pos_x=0.0f;
            raster_pos_y+=(float)m_iLineHeight;
            if(nLines==0)
                linebuffer[nLines]=nCharsOnLine;
            else
                linebuffer[nLines]=linebuffer[nLines-1]+nCharsOnLine;

            bLineStarted=false;
            nCharsOnLine=0;
            nLines++;
            continue;
        }
        else if(actual_char=='\t')
        {
            raster_pos_x+=(m_pszCharW[32-m_iBaseChar]*5);
            continue;
        }
        if(ch<0 || ch>=m_nChars)
        {
            continue;
        }
        
        /*
        if(raster_pos_x+(float)m_pszCharD[ch]+kerning>cx)
        {
            raster_pos_x=0.0f;
            raster_pos_y+=(float)m_iLineHeight;
            if(nLines==0)
                linebuffer[nLines]=nCharsOnLine;
            else
                linebuffer[nLines]=linebuffer[nLines-1]+nCharsOnLine;

            bLineStarted=false;
            nCharsOnLine=0;
            nLines++;
        }
        */
        
        if(!bLineStarted)
        {
            if(actual_char!=32)
                bLineStarted=true;
        }
        
        int offx=m_pszCharOffsetX[ch];
        int offy=m_pszCharOffsetY[ch];
        
        float left=raster_pos_x+offx;
        float right=raster_pos_x+m_pszCharW[ch]+offx;
        float top=raster_pos_y+offy;
        float bottom=raster_pos_y+m_pszCharH[ch]+offy;
        
        vertexCoordBuffer[cntr*12+0]=(short)right;     vertexCoordBuffer[cntr*12+1]=(short)top;
        vertexCoordBuffer[cntr*12+2]=(short)left;      vertexCoordBuffer[cntr*12+3]=(short)top;
        vertexCoordBuffer[cntr*12+4]=(short)right;     vertexCoordBuffer[cntr*12+5]=(short)bottom;
        vertexCoordBuffer[cntr*12+6]=(short)left;      vertexCoordBuffer[cntr*12+7]=(short)bottom;
        vertexCoordBuffer[cntr*12+8]=(short)right;     vertexCoordBuffer[cntr*12+9]=(short)bottom;
        vertexCoordBuffer[cntr*12+10]=(short)left;     vertexCoordBuffer[cntr*12+11]=(short)top;
        raster_pos_x+=((float)m_pszCharD[ch]+kerning);
        
        textureCoordBuffer[cntr*12+0]=(m_pszU[ch]+m_pszU_width[ch]);   textureCoordBuffer[cntr*12+1]=m_pszV[ch];
        textureCoordBuffer[cntr*12+2]=m_pszU[ch];                      textureCoordBuffer[cntr*12+3]=m_pszV[ch];
        textureCoordBuffer[cntr*12+4]=(m_pszU[ch]+m_pszU_width[ch]);   textureCoordBuffer[cntr*12+5]=(m_pszV[ch]+m_pszU_height[ch]);
        textureCoordBuffer[cntr*12+6]=m_pszU[ch];                      textureCoordBuffer[cntr*12+7]=(m_pszV[ch]+m_pszU_height[ch]);
        textureCoordBuffer[cntr*12+8]=(m_pszU[ch]+m_pszU_width[ch]);   textureCoordBuffer[cntr*12+9]=(m_pszV[ch]+m_pszU_height[ch]);
        textureCoordBuffer[cntr*12+10]=m_pszU[ch];                     textureCoordBuffer[cntr*12+11]=m_pszV[ch];
        
        cntr++;
        nCharsOnLine++;
    }
    nChar=cntr;
    if(nCharsOnLine>0)
        nLines++;
}

bool geFont::canNeglectNextWord(const char* str, int cx, int nChar, int iCurIndex, float raster_pos_x) const
{
    float kerning=GE_DEBUG_KERNING;
    //float last_space_charXPos=0.0f;
    bool bWordStarted=false;
    
    int x=0;
    for(x=iCurIndex;x<nChar;x++)
    {
        int actual_char=(unsigned char)str[x];
        int ch=actual_char-m_iBaseChar;

        if(!bWordStarted)
        {
            if(actual_char==32)
                continue;
            else
                bWordStarted=true;
        }
        
        if(actual_char==32) //space
        {
            return false;
        }
        
        if(actual_char=='\n')
        {
            return false;
        }
        if(actual_char=='\t')
        {
            return false;
        }
//        if(actual_char=='.')
//        {
//            return false;
//        }
        if(ch<0 || ch>=m_nChars)
        {
            continue;
        }
        
        if(raster_pos_x+(float)m_pszCharD[ch]+kerning>cx)
        {
            if(actual_char==32 || actual_char=='\n' || actual_char=='\t')
            {
                return false;
            }
            raster_pos_x=0.0f;
            break; //found eol
        }
        
        raster_pos_x+=((float)m_pszCharD[ch]+kerning);
    }
    
    if(x==nChar)
        return false;
    
    return true;
}

//========================================================================================

geFontManager geFontManager::g_cFontManager;
geFont* geFontManager::g_pFontArial10_84Ptr=NULL;
geFont* geFontManager::g_pFontArial10_80Ptr=NULL;
    
void geFontManager::InitializeFonts()
{
    g_cFontManager.init();
    
//    g_pFontArial10_84Ptr=g_cFontManager.loadFont("res//fonts//arial_iphone10_84.ecf");
//    g_pFontArial10_80Ptr=g_cFontManager.loadFont("res//fonts//arial_iphone10_80.ecf");
    g_pFontArial10_84Ptr=g_cFontManager.loadFont(cpp_getPath("/res/fonts/arial_iphone10_84.ecf").c_str());
    g_pFontArial10_80Ptr=g_cFontManager.loadFont(cpp_getPath("/res/fonts/arial_iphone10_80.ecf").c_str());

    
    if (g_pFontArial10_84Ptr) {
        g_pFontArial10_84Ptr->setRGBA(0.9f, 0.9f, 0.9f);
    }
    if (g_pFontArial10_80Ptr) {
        g_pFontArial10_80Ptr->setRGBA(0.5f, 0.5f, 0.5f);
    }
}
    
geFontManager::geFontManager()
{
#if defined (USE_ProgrammablePipeLine)
	m_pRendererPtr=NULL;
#endif
}

geFontManager::~geFontManager()
{
	reset(false);
}

void geFontManager::init()
{
#if defined (USE_ProgrammablePipeLine)
    //if(technique==rendererBase::gl_programmable_pipeline)
    {
#if GEAR_WINDOWS
        m_cFontShader.loadShader(cpp_getPath("res/shadersWin32/hwshader/fontshader.glsl"));
#elif GEAR_ANDROID
        m_cFontShader.loadShader(cpp_getPath("/res/shadersWin32/hwshader/fontshader.glsl"));
#elif GEAR_APPLE
        //resource_dir_root_path="./Contents/Resources/res/shadersWin32/";
        m_cFontShader.loadShader(cpp_getPath("/res/shadersWin32/hwshader/fontshader.glsl"));
#endif
    }
#endif
}

void geFontManager::reset(bool reload)
{
    for(int x=0;x<m_cvFontList.size();x++)
    {
        geFont* font=m_cvFontList[x];
        if(reload)font->setDeleteGLTexture(!reload);
        GX_DELETE(font);
    }
    m_cvFontList.clear();
}

#if defined (USE_ProgrammablePipeLine)
void geFontManager::setRenderer(gxRenderer* renderer)
{
	m_pRendererPtr=renderer;
	for(int x=0;x<m_cvFontList.size();x++)
    {
        geFont* font=m_cvFontList[x];
		font->setRenderer(m_pRendererPtr);
    }
}
#endif
    
geFont* geFontManager::loadFont(const char* filename)
{
#if defined (USE_ProgrammablePipeLine)
    geFont* newFont=new geFont(&m_cFontShader, m_pRendererPtr);
#else
    geFont* newFont=new geFont();
#endif
    gxFile file;
    
    std::string orig_filename = filename;
    if(filename[0]=='.')
    {
        orig_filename = cpp_getCurrentWorkingDirectory();
        orig_filename+=&filename[1];
    }
    
    if (file.OpenFile(orig_filename.c_str(), gxFile::FILE_r)) {
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("font %s loading...", orig_filename.c_str());
#endif 
        newFont->load(file);
        file.CloseFile();
        m_cvFontList.push_back(newFont);
    } else {
        GX_DELETE(newFont);
        newFont = nullptr;
    }
    return newFont;
}

