#pragma once

#define PNG_feature_SUPPORTED
#include "../../libpng16/png.h"
//#include "../core/basicIncludes.h"
//#include "gxDebug.h"

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <stdarg.h>

#include <assert.h>
#ifdef GEAR_WINDOWS
#include <malloc.h>
#include <conio.h>
#elif GEAR_APPLE
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif
#include "../OSUtils.h"

int read_png_file(const char* file_name, bool& bAlpha, unsigned int& cx, unsigned int& cy, unsigned int& bpp)
{
	int width, height;
	png_byte color_type;
//	png_byte color_bpp;
	png_byte bit_depth;
    png_byte header[8];    // 8 is the maximum size that can be checked

    std::string orig_filename;
    orig_filename = file_name;
    if(file_name[0]=='.')
    {
        orig_filename = OSUtils::cpp_getCurrentWorkingDirectory();
        orig_filename+=&file_name[1];
    }
    
    /* open file and test for it being a png */
    FILE *fp = fopen(orig_filename.c_str(), "rb");
    if (!fp)
    {
		//DEBUG_PRINT("[read_png_file] File %s could not be opened for reading", file_name);
		return 0;
	}
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8))
    {
		fclose(fp);
		//DEBUG_PRINT("[read_png_file] File %s is not recognized as a PNG file", file_name);
		return 0;
	}


    /* initialize stuff */
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr)
    {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		//DEBUG_PRINT("[read_png_file] png_create_read_struct failed");
		return 0;
	}

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		//DEBUG_PRINT("[read_png_file] png_create_info_struct failed");
		return 0;
	}

	//create png info struct
	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info)
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
		//DEBUG_PRINT("Unable to create png end info : %s");
		return 0;
	}

    if (setjmp(png_jmpbuf(png_ptr)))
    {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		//DEBUG_PRINT("[read_png_file] Error during init_io");
		return 0;
	}

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);

	//png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    png_read_info(png_ptr, info_ptr);

    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    //number_of_passes = png_set_interlace_handling(png_ptr);
    //png_read_update_info(png_ptr, info_ptr);

	/* extract multiple pixels with bit depths of 1, 2, and 4 from a single
	* byte into separate bytes (useful for paletted and grayscale images).
	*/
	png_set_packing(png_ptr);


	/* expand paletted colors into true RGB triplets */
	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_expand(png_ptr);

	/* expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel */
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand(png_ptr);

	/* expand paletted or RGB images with transparency to full alpha channels
	* so the data will be available as RGBA quartets */
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_expand(png_ptr);


    png_read_update_info(png_ptr, info_ptr);

    color_type = png_get_color_type(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    ///* read file */
    if (setjmp(png_jmpbuf(png_ptr)))
    {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		//DEBUG_PRINT("[read_png_file] Error during read_image");
		return 0;
	}

//	int out_format=0;

	int rowbytes = (int)png_get_rowbytes(png_ptr, info_ptr);

	// Allocate the image_data as a big block, to be given to opengl
	png_byte *image_data = new png_byte[rowbytes * height];
	if (!image_data)
	{
		fclose(fp);
		//clean up memory and close stuff
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		//LOGE("Unable to allocate image_data while loading %s ", filename);
		//DEBUG_PRINT("Unable to allocate image_data while loading");
		return 0;
	}

    png_bytep* row_pointers = new  png_bytep[height];
	if  (!row_pointers)
	{
		fclose(fp);
		//clean up memory and close stuff
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		delete[] image_data;
		//DEBUG_PRINT("Unable to allocate row_pointer while loading");
		return 0;
	}
	// set the individual row_pointers to point at the correct offsets of image_data
	//for (int i = 0; i < height; ++i)
	//	row_pointers[i/*height - 1 - i*/] = image_data + i * rowbytes;
	for (int i = height-1; i >= 0; i--)
		row_pointers[i/*height - 1 - i*/] = image_data + i * rowbytes;

    png_read_image(png_ptr, row_pointers);

    unsigned int texID=0;
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
        
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	cx=width;
	cy=height;

	if(color_type==PNG_COLOR_TYPE_RGB)
	{
		bpp=3;
		bAlpha=false;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
	}
	else if(color_type==PNG_COLOR_TYPE_RGB_ALPHA)
	{
		bpp=4;
		bAlpha=true;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	}
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);


	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	delete[] image_data;
	free(row_pointers);
    fclose(fp);

	////DEBUG_PRINT("%s : size(%d, %d), format %d, compressed size %d bytes\n", out_filename, width, height, out_format, compressedSize);
	return texID;
}
