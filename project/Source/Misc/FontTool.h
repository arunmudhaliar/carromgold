#ifndef ECFONTTOOL_H
#define ECFONTTOOL_H

//FreeType Headers
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

//Using the STL exception library increases the
//chances that someone else using our code will corretly
//catch any exceptions that we throw.
#include <stdexcept>

//#include <Windows.h>
//#include <gl/GL.h>
#include <OpenGL/gl.h>

struct stCharCap
{
	stCharCap()
	{
		xoffset=0;
		yoffset=0;
		width=0;
		height=0;
		bmp_width=bmp_height=0;
		displace=0;
		uv_x=uv_y=0.0f;
		rgba[0]=rgba[1]=rgba[2]=rgba[3]=0.0f;
		bSelected=false;
		next=previous=NULL;
		buffer=NULL;
	}

	~stCharCap()
	{
		if(buffer)
		{
			delete [] buffer;
			buffer=NULL;
		}
	}

	short xoffset;
	short yoffset;
	unsigned short width;		//in 2th power
	unsigned short height;		//in 2th power
	unsigned short bmp_width;	//original bmp width
	unsigned short bmp_height;	//original bmp height
	short displace;

	float uv_x;
	float uv_y;

	float rgba[4];
	bool bSelected;
	unsigned char* buffer;
	stCharCap* next;
	stCharCap* previous;
};

class ECFontTool
{
public:
	ECFontTool();
	~ECFontTool();

	void init();
	void reset();
	void load();

	//unsigned int getFontBase()		{	return m_uFontBase;	}
	stCharCap* getCharCap(int index)	{	return m_pszCharCaps[index];	}
	unsigned int getTex(int index)		{	return m_pszTexture[index];		}

	unsigned int getNoOfChars()			{	return m_nChars;	}
	unsigned int getTallestCharHeight()	{	return m_iTallestCharHeight;	}
private:

	void allocateMemory(int nChars);

	int next_p2(int a);
	void make_dlist(FT_Face face, int ch, GLuint list_base, GLuint* tex_base);

	//unsigned int m_uFontBase;
	unsigned int m_nChars;
	unsigned int* m_pszTexture;
	stCharCap** m_pszCharCaps;
	unsigned int m_iTallestCharHeight;
};

#endif
