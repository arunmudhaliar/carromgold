#include "FontTool.h"

ECFontTool::ECFontTool()
{
}

ECFontTool::~ECFontTool()
{
	if(m_pszTexture)
	{
		glDeleteTextures(m_nChars, m_pszTexture);
		delete [] m_pszTexture;
	}

	if(m_pszCharCaps)
	{
		for(int x=0;x<m_nChars;x++)
		{
			delete m_pszCharCaps[x];
		}

		delete [] m_pszCharCaps;
	}
}

void ECFontTool::init()
{
	m_iTallestCharHeight=0;
	m_nChars=0;
	m_pszTexture=NULL;
	m_pszCharCaps=NULL;
	//memset(m_uTexture, 0, sizeof(m_uTexture));
	//m_uFontBase=0;
}

void ECFontTool::reset()
{
}

void ECFontTool::allocateMemory(int nChars)
{
	m_nChars=nChars;
	m_pszTexture=new unsigned int[nChars];
	memset(m_pszTexture, 0, sizeof(unsigned int)*nChars);
	m_pszCharCaps= new stCharCap*[nChars];
	stCharCap* previous=NULL;

	for(int x=0;x<nChars;x++)
	{
		m_pszCharCaps[x]=new stCharCap();
		if(previous)previous->next=m_pszCharCaps[x];
		m_pszCharCaps[x]->previous=previous;
		previous=m_pszCharCaps[x];
	}
	//memset(m_pszCharCaps, 0, sizeof(stCharCap)*nChars);
}

void ECFontTool::load()
{
	int h=22;//s16;//22;
	//Create and initilize a freetype font library.
	FT_Library library;
	if (FT_Init_FreeType( &library )) 
		throw std::runtime_error("FT_Init_FreeType failed");

	//The object in which Freetype holds information on a given
	//font is called a "face".
	FT_Face face;

	//This is where we load in the font information from the file.
	//Of all the places where the code might die, this is the most likely,
	//as FT_New_Face will die if the font file does not exist or is somehow broken.
	if (FT_New_Face( library, "testData\\Arial_iphone.ttf", 0, &face )) 
		throw std::runtime_error("FT_New_Face failed (there is probably a problem with your font file)");

	//For some twisted reason, Freetype measures font size
	//in terms of 1/64ths of pixels.  Thus, to make a font
	//h pixels high, we need to request a size of h*64.
	//(h << 6 is just a prettier way of writting h*64)
	FT_Set_Char_Size( face, h << 6, h << 6, 75, 75);	//72 is for normal...but crashes on some fonts

	allocateMemory(8192);
	//TODO
	//Here we ask opengl to allocate resources for
	//all the textures and displays lists which we
	//are about to create.  
	//m_uFontBase=glGenLists(m_nChars);
	glGenTextures( m_nChars, m_pszTexture );
	//This is where we actually create each of the fonts display lists.
	unsigned int tallest_char_height=0;
	int min_h_bound=0;
	int max_h_bound=0;
	for(unsigned int i=0;i<m_nChars;i++)
	{
		make_dlist(face, i, 0, m_pszTexture);

		if(m_pszCharCaps[i]->yoffset<min_h_bound)
			min_h_bound=m_pszCharCaps[i]->yoffset;
		if(m_pszCharCaps[i]->bmp_height+m_pszCharCaps[i]->yoffset>max_h_bound)
			max_h_bound=m_pszCharCaps[i]->bmp_height+m_pszCharCaps[i]->yoffset;
		//if(m_pszCharCaps[i]->bmp_height>tallest_char_height)
		//	tallest_char_height=m_pszCharCaps[i]->bmp_height;
	}
	m_iTallestCharHeight=max_h_bound-min_h_bound;
	//

	//We don't need the face information now that the display
	//lists have been created, so we free the assosiated resources.
	FT_Done_Face(face);

	//Ditto for the library.
	FT_Done_FreeType(library);
}

///This function gets the first power of 2 >= the
///int that we pass it.
int ECFontTool::next_p2(int a)
{
	int rval=1;
	while(rval<a) rval<<=1;
	return rval;
}
///Create a display list coresponding to the give character.
void ECFontTool::make_dlist(FT_Face face, int ch, GLuint list_base, GLuint* tex_base)
{
	//The first thing we do is get FreeType to render our character
	//into a bitmap.  This actually requires a couple of FreeType commands:

	//Load the Glyph for our character.
	if(FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), FT_LOAD_DEFAULT ))
		throw std::runtime_error("FT_Load_Glyph failed");

	//Move the face's glyph into a Glyph object.
    FT_Glyph glyph;
    if(FT_Get_Glyph( face->glyph, &glyph ))
		throw std::runtime_error("FT_Get_Glyph failed");

	//Convert the glyph to a bitmap.
	FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
    FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

	//This reference will make accessing the bitmap easier
	FT_Bitmap& bitmap=bitmap_glyph->bitmap;

	//Use our helper function to get the widths of
	//the bitmap data that we will need in order to create
	//our texture.

	if(ch==32)
	{
		bitmap.rows=5;
		bitmap.width=face->glyph->advance.x >> 6;
	}

	int width = next_p2( bitmap.width );
	int height = next_p2( bitmap.rows );

	//Allocate memory for the texture data.
	GLubyte* expanded_data = new GLubyte[ 2 * width * height];

	//Here we fill in the data for the expanded bitmap.
	//Notice that we are using two channel bitmap (one for
	//luminocity and one for alpha), but we assign
	//both luminocity and alpha to the value that we
	//find in the FreeType bitmap. 
	//We use the ?: operator so that value which we use
	//will be 0 if we are in the padding zone, and whatever
	//is the the Freetype bitmap otherwise.
	for(int j=0; j <height;j++)
	{
		for(int i=0; i < width; i++)
		{
			expanded_data[2*(i+j*width)]= expanded_data[2*(i+j*width)+1] = 
				(i>=bitmap.width || j>=bitmap.rows || ch==32) ?
				0 : bitmap.buffer[i + bitmap.width*j];
		}
	}


	//Now we just setup some texture paramaters.
    glBindTexture( GL_TEXTURE_2D, tex_base[ch]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

	//Here we actually create the texture itself, notice
	//that we are using GL_LUMINANCE_ALPHA to indicate that
	//we are using 2 channel data.
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height,
		  0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data );

	//With the texture created, we don't need to expanded data anymore
    delete [] expanded_data;

	//So now we can create the display list
	//glNewList(list_base+ch,GL_COMPILE);

	glBindTexture(GL_TEXTURE_2D, 0);

	//glPushMatrix();

	//first we need to move over a little so that
	//the character has the right amount of space
	//between it and the one before it.
	//glTranslatef(bitmap_glyph->left,0,0);
	m_pszCharCaps[ch]->xoffset=bitmap_glyph->left;
	//Now we move down a little in the case that the
	//bitmap extends past the bottom of the line 
	//(this is only true for characters like 'g' or 'y'.
	//glTranslatef(0,bitmap_glyph->top-bitmap.rows,0);
	//glTranslatef(0,bitmap_glyph->top-bitmap.rows,0);
	m_pszCharCaps[ch]->yoffset=-bitmap_glyph->top;//-bitmap.rows;
	//Now we need to account for the fact that many of
	//our textures are filled with empty padding space.
	//We figure what portion of the texture is used by 
	//the actual character and store that information in 
	//the x and y variables, then when we draw the
	//quad, we will only reference the parts of the texture
	//that we contain the character itself.
	float	x=(float)bitmap.width / (float)width,
			y=(float)bitmap.rows / (float)height;

	m_pszCharCaps[ch]->buffer=new unsigned char[bitmap.rows*bitmap.width*2];
	for(int s=0;s<bitmap.rows;s++)
	{
		for(int u=0;u<bitmap.width;u++)
		{
			m_pszCharCaps[ch]->buffer[2*(s*bitmap.width+u)+0]= (ch!=32)?bitmap.buffer[s*bitmap.width+u]:0;
			m_pszCharCaps[ch]->buffer[2*(s*bitmap.width+u)+1]= (ch!=32)?bitmap.buffer[s*bitmap.width+u]:0;
		}
	}

	m_pszCharCaps[ch]->bmp_width=bitmap.width;
	m_pszCharCaps[ch]->bmp_height=bitmap.rows;

	m_pszCharCaps[ch]->uv_x=x;
	m_pszCharCaps[ch]->uv_y=y;
	//Here we draw the texturemaped quads.
	//The bitmap that we got from FreeType was not 
	//oriented quite like we would like it to be,
	//so we need to link the texture to the quad
	//so that the result will be properly aligned.
	//glBegin(GL_QUADS);
	//glTexCoord2d(0,0); glVertex2f(0,bitmap.rows);
	//glTexCoord2d(0,y); glVertex2f(0,0);
	//glTexCoord2d(x,y); glVertex2f(bitmap.width,0);
	//glTexCoord2d(x,0); glVertex2f(bitmap.width,bitmap.rows);
	//glEnd();
	//glPopMatrix();
	//glTranslatef(face->glyph->advance.x >> 6 ,0,0);

	m_pszCharCaps[ch]->displace=face->glyph->advance.x >> 6;
	m_pszCharCaps[ch]->width=bitmap.width;
	m_pszCharCaps[ch]->height=bitmap.rows;
	//increment the raster position as if we were a bitmap font.
	//(only needed if you want to calculate text length)
	//glBitmap(0,0,0,0,face->glyph->advance.x >> 6,0,NULL);

	//Finnish the display list
	//glEndList();
}
