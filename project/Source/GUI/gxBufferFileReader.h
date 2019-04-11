#ifndef GXBUFFERFILEREADER_H
#define GXBUFFERFILEREADER_H

#include <stdio.h>
#include <string>

#ifdef ANDROID
	typedef long long __int64;
#endif

#ifdef _WIN32
class DECLSPEC gxBufferFileReader
#else
class gxBufferFileReader
#endif
{
public:
	gxBufferFileReader(const std::string& buffer);
    gxBufferFileReader(const std::string& buffer, long size);
	~gxBufferFileReader();

    void Rewind();
    
//    void    Seek(unsigned int off, int flag) const;
	long    Tell() const;

	void	Read(char& v);
//    char* ReadString();
	void	Read(unsigned char& v);
	void	Read(bool& v);
	void	Read(short& v);
	void	Read(unsigned short& v);
	void	Read(int& v);
	void	Read(unsigned int& v);
	void	Read(float& v);
	void	Read(double& v);
    void    Read(unsigned long& v);
#if !defined(__APPLE__)
	void	Read(__int64& v);
#endif
    void    ReadBuffer(unsigned char* buffer, unsigned long cnt);
    
private:
    const std::string& fileBuffer;
    
private:
    bool CanRead(long sz);
    long filePointer;
    long fileSize;
    const char* fileStartPtr;
};

#endif
