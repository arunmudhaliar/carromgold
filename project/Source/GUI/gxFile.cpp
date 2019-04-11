#include "gxFile.h"
#include <string.h>
#include <errno.h>
#include "gxDebug.h"
#include "OSUtils.h"

#if GEAR_ANDROID
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#endif


gxFile::gxFile()
{
	m_eFileMode=FILE_r;
	m_pFP=NULL;
}

gxFile::~gxFile()
{
	CloseFile();
}

bool gxFile::GetDataBuffer(const std::string& filename, std::string& buffer, long& size) {
    //read shader code
    size=0;
#if GEAR_APPLE
    FILE* fp=fopen(filename.c_str(), "r");
    if(fp==NULL) {
        DEBUG_PRINT("FILE open ERROR %s", filename.c_str());
        return false;
    }
    
    fseek(fp, 0, SEEK_END);
    size=(int)ftell(fp);
    fclose(fp);
    
    if(!size) {
        DEBUG_PRINT("FILE size ZERO ERROR %s", filename.c_str());
        return false;
    }
    
    //
    fp=fopen(filename.c_str(), "r");
    if(fp==NULL) {
        DEBUG_PRINT("FILE open ERROR #2 %s", filename.c_str());
        return false;
    }
    
    char* data=new char[size+1];
    if (!data) {
        DEBUG_PRINT("GetDataBuffer() - data allocation ERROR #3 - %s", filename.c_str());
        fclose(fp);
        return false;
    }
    
    fread((void*)data, 1, size, fp);
    data[size] = '\0';
    fclose(fp);

    buffer.assign(data, size);
    delete [] data;
    return true;
#elif GEAR_ANDROID
    AAssetManager* mgr = OSUtils::cpp_getAssetManager();
    AAsset* asset = AAssetManager_open(mgr, filename.c_str(), AASSET_MODE_BUFFER);
    if(!asset) {
        DEBUG_PRINT("AAssetManager_open() - failed : %s", filename.c_str());
        return false;
    }
    
    size = (long)AAsset_getLength64(asset);
    char* data=new char[size+1];
    if (!data) {
        DEBUG_PRINT("GetDataBuffer() - data allocation error - %s", filename.c_str());
        AAsset_close(asset);
        return false;
    }
    
    if (AAsset_read(asset, data, size) < 0) {
        delete [] data;
        AAsset_close(asset);
        DEBUG_PRINT("AAsset_read() - failed : %s", filename.c_str());
        return false;
    }
    
    AAsset_close(asset);
    
    data[size] = '\0';
    buffer.assign(data, size);
    delete [] data;
    
    return true;
#endif
}

int gxFile::OpenFile(const std::string& filename, EFILEMODE eFileMode/* =FILE_r */)
{
	//int err=0;
	m_eFileMode=eFileMode;
	
	switch(m_eFileMode)
	{
#ifdef _WIN32
		case FILE_r: fopen_s(&m_pFP, filename.c_str(), "rb");	break;
		case FILE_w: fopen_s(&m_pFP, filename.c_str(), "wb");	break;
		case FILE_a: fopen_s(&m_pFP, filename.c_str(), "a");		break;
#else
		case FILE_r: m_pFP = fopen(filename.c_str(), "rb");		break;
		case FILE_w: m_pFP = fopen(filename.c_str(), "wb");		break;
		case FILE_a: m_pFP = fopen(filename.c_str(), "a");		break;
#endif
	}

	if(m_pFP==NULL)
    {        
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("Error opening file - %s, %s", filename.c_str(), strerror(errno));
#endif 
        
    }
	
	return (m_pFP)?1:0;
}

int gxFile::OpenFileDescriptor(int fd, EFILEMODE eFileMode)
{
	//int err=0;
	m_eFileMode=eFileMode;
	
	switch(m_eFileMode)
	{
#ifdef _WIN32
		case FILE_r: m_pFP = _fdopen(fd, "rb");		break;
		case FILE_w: m_pFP = _fdopen(fd, "wb");		break;
		case FILE_a: m_pFP = _fdopen(fd, "a");		break;
#else
		case FILE_r: m_pFP = fdopen(fd, "rb");		break;
		case FILE_w: m_pFP = fdopen(fd, "wb");		break;
		case FILE_a: m_pFP = fdopen(fd, "a");		break;
#endif
	}

    if(m_pFP==NULL)
    {        
#if LOG_DEBUG_ENGINE
        DEBUG_PRINT("Error opening file - %s", strerror(errno));
#endif 
    }
	return (m_pFP)?1:0;
}

void gxFile::Seek(unsigned int off, int flag) const
{
	fseek(m_pFP, off, flag);
}

long gxFile::Tell() const
{
    return ftell(m_pFP);
}

void gxFile::CloseFile()
{
	if(m_pFP)
    {
//#ifndef ANDROID
        if(fclose(m_pFP))
        {
#if LOG_DEBUG_ENGINE
            DEBUG_PRINT("Error closing file - %s", strerror(errno));
#endif 
            
        }
//#endif
    }
	m_pFP=NULL;
}

void gxFile::Read(char& v) const
{
	fread((void*)&v, sizeof(char), 1, m_pFP);
}

char* gxFile::ReadString() const
{
	char* aVar=NULL;
	int aCount;
	Read(aCount);
	aVar=new char[aCount+1];	
	fread(aVar, aCount, 1, m_pFP);
	aVar[aCount]='\0';						//This is a must

	return aVar;
}

void gxFile::Read(unsigned char& v) const
{
	fread((void*)&v, sizeof(unsigned char), 1, m_pFP);
}

void gxFile::Read(bool& v) const
{
	fread((void*)&v, sizeof(bool), 1, m_pFP);
}

void gxFile::Read(short& v) const
{
	fread((void*)&v, sizeof(short), 1, m_pFP);
}

void gxFile::Read(unsigned short& v) const
{
	fread((void*)&v, sizeof(unsigned short), 1, m_pFP);
}

void gxFile::Read(int& v) const
{
	fread((void*)&v, sizeof(int), 1, m_pFP);
}

void gxFile::Read(unsigned int& v) const
{
	fread((void*)&v, sizeof(unsigned int), 1, m_pFP);
}

void gxFile::Read(float& v) const
{
	fread((void*)&v, sizeof(float), 1, m_pFP);
}

void gxFile::Read(double& v) const
{
	fread((void*)&v, sizeof(double), 1, m_pFP);
}

void gxFile::Read(unsigned long& v) const
{
	fread((void*)&v, sizeof(unsigned long), 1, m_pFP);    
}

#if !defined(__APPLE__)
void gxFile::Read(__int64& v) const
{
	fread((void*)&v, sizeof(__int64), 1, m_pFP);
}
#endif

void gxFile::ReadBuffer(unsigned char* buffer, unsigned long cnt) const
{
    if(!fread((void*)buffer, cnt, 1, m_pFP)) {
        DEBUG_PRINT("ReadBuffer failed");
    }
}

//----------------Write Functions-----------------
void gxFile::Write(char v) const
{
	Write(&v, sizeof(char));
}

void gxFile::Write(const char		aVar[]) const
{
	int aCount;
	aCount=(int)strlen(aVar);					//Get the length of the String
	Write(aCount);							//Write the string length on to the stream
	Write(aVar,aCount);						//Write the String on to the stream
}

void gxFile::Write(unsigned char v) const
{
	Write(&v, sizeof(unsigned char));
}

void gxFile::Write(bool v) const
{
	Write(&v, sizeof(bool));
}

void gxFile::Write(short v) const
{
	Write(&v, sizeof(short));
}

void gxFile::Write(unsigned short v) const
{
	Write(&v, sizeof(unsigned short));
}

void gxFile::Write(int v) const
{
	Write(&v, sizeof(int));
}

void gxFile::Write(unsigned int v) const
{
	Write(&v, sizeof(unsigned int));
}

void gxFile::Write(float v) const
{
	Write(&v, sizeof(float));
}

void gxFile::Write(double v) const
{
	Write(&v, sizeof(double));
}

#if !defined(__APPLE__)
void gxFile::Write(__int64 v) const
{
	Write(&v, sizeof(__int64));
}
#endif

void gxFile::WriteBuffer(unsigned char* buffer, unsigned long cnt) const
{
	fwrite((char*)buffer, cnt, 1, m_pFP);
}

void gxFile::Write(const void* pAny, int iCount) const
{
	fwrite((char*)pAny, iCount, 1, m_pFP);
}
