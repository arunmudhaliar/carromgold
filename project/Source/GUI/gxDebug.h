#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
//#include "basicIncludes.h"

#if defined(GEAR_WINDOWS) && defined(GEAR_DEBUG)
	#include <DbgHelp.h>
	#define LOG_GL_ERROR
#endif

extern "C" void DEBUG_PRINT(const char* format, ...);



#ifdef LOG_GL_ERROR
//GL ERROR
extern "C" const char* GetGLErrorName(GLenum errorValue);
extern "C" void printCallStack();

//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wformat-security"
#define CHECK_GL_ERROR(fn)																			\
	do																								\
	{																								\
		fn;																							\
		GLenum error = glGetError();																\
		if (error != GL_NO_ERROR)																	\
		{																							\
			char errorMsg[FILENAME_MAX];																	\
			_snprintf(errorMsg, FILENAME_MAX, #fn " failed with glError %s\n%s (%d)", GetGLErrorName(error), __FILE__, __LINE__);	\
			DEBUG_PRINT(errorMsg);																	\
			printCallStack();																		\
		}																							\
	} while(0)
//#pragma GCC diagnostic pop

#else
#define CHECK_GL_ERROR(fn) fn
#endif
