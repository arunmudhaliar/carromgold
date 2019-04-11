#include "gxDebug.h"

#if defined(GEAR_ANDROID)
#include <android/log.h>
#endif
//#define  LOG_TAG    "gearApp-native"
//#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
//#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
//#define  LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))

extern "C"
{
void DEBUG_PRINT(const char* format, ...)
{
	char buffer[FILENAME_MAX];
	va_list v;
	va_start(v,format);
	vsprintf(buffer, format, v);
#ifdef GEAR_WINDOWS
	printf("%s\n", buffer);
#elif defined(GEAR_ANDROID)
	 __android_log_print(ANDROID_LOG_INFO,"carromgold", "%s", buffer);
#else
	printf("%s\n", buffer);
#endif
	va_end(v);
}

#ifdef LOG_GL_ERROR
const char* GetGLErrorName(GLenum errorValue)
{
	switch (errorValue)
	{
		case GL_NO_ERROR:
			return "GL_NO_ERROR";
		case GL_INVALID_ENUM:
			return "GL_INVALID_ENUM";
		case GL_INVALID_VALUE:
			return "GL_INVALID_VALUE";
		case GL_INVALID_OPERATION:
			return "GL_INVALID_OPERATION";
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			return "GL_INVALID_FRAMEBUFFER_OPERATION";
		case GL_OUT_OF_MEMORY:
			return "GL_OUT_OF_MEMORY";
		default:
			return "UNKNOWN";
	}
}


void printCallStack()
{
	typedef USHORT (WINAPI *CaptureStackBackTraceType)(__in ULONG, __in ULONG, __out PVOID*, __out_opt PULONG);
	CaptureStackBackTraceType func = (CaptureStackBackTraceType)(GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "RtlCaptureStackBackTrace"));

	if(func == NULL)
		return; // WOE 29.SEP.2010

	//called this on app startup
	//SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
	//SymInitialize(GetCurrentProcess(), NULL, TRUE);
	//

	printf("Call stack\n");
	// Quote from Microsoft Documentation:
	// ## Windows Server 2003 and Windows XP:  
	// ## The sum of the FramesToSkip and FramesToCapture parameters must be less than 63.
	const int kMaxCallers = 62; 

	void* callers[kMaxCallers];
	int count = (func)(0, kMaxCallers, callers, NULL);
	for(int g = 0; g < count; g++)
	{	   
		//https://code.google.com/p/cuda-waste/source/browse/trunk/wrapper/call-stack-info.cpp?spec=svn193&r=193
		IMAGEHLP_LINE  lineInfo;
		char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
		PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
		//SYMBOL_INFO symbol;
		DWORD		dwDisp=0;
		DWORD64		dwDisp64=0;

		//ZeroMemory( &symbol, sizeof( symbol ) );
		pSymbol->MaxNameLen = 256;
		pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);

		ZeroMemory( &lineInfo, sizeof( lineInfo ) );
		lineInfo.SizeOfStruct = sizeof( lineInfo );
		if(SymFromAddr(GetCurrentProcess(), (DWORD)(callers[g]), &dwDisp64, pSymbol))
		{
			printf("Function %s() ", pSymbol->Name);
		}

		if ( SymGetLineFromAddr( GetCurrentProcess(), (DWORD)callers[g], &dwDisp, &lineInfo ) )
		{
			char * fn = lineInfo.FileName;
			printf("file %s, line %d\n", fn, lineInfo.LineNumber);
		}
		else
		{
			printf("*** %d called from %016I64LX\n", g, callers[g]);
		}
	}

	printf("\n");

	//called this on app 
	//SymCleanup(GetCurrentProcess());
}
#endif

}
