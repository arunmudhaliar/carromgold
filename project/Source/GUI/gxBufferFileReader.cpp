#include "gxBufferFileReader.h"
#include <string.h>
#include <errno.h>
#include "gxDebug.h"

gxBufferFileReader::gxBufferFileReader(const std::string& buffer, long size) :
fileBuffer(buffer),
fileSize(size),
filePointer(0),
fileStartPtr(nullptr) {
    assert(fileSize==fileBuffer.size());
    if (fileSize) {
        fileStartPtr = buffer.c_str();
    }
}

gxBufferFileReader::gxBufferFileReader(const std::string& buffer) :
fileBuffer(buffer),
filePointer(0),
fileStartPtr(nullptr) {
    fileSize = fileBuffer.size();
    if (fileSize) {
        fileStartPtr = buffer.c_str();
    }
}

gxBufferFileReader::~gxBufferFileReader() {
}

void gxBufferFileReader::Rewind() {
    filePointer=0;
}

long gxBufferFileReader::Tell() const {
    return fileBuffer.size();
}

bool gxBufferFileReader::CanRead(long sz) {
    return ((filePointer+sz) <= fileSize);
}

void gxBufferFileReader::Read(char& v) {
    assert(CanRead(sizeof(char)));
//    v = *(char*)(fileStartPtr+filePointer);
    memcpy((void*)&v, (void*)(fileStartPtr+filePointer), sizeof(char));
    filePointer+=sizeof(char);
}

//char* gxFile::ReadString() const
//{
//    char* aVar=NULL;
//    int aCount;
//    Read(aCount);
//    aVar=new char[aCount+1];
//    fread(aVar, aCount, 1, m_pFP);
//    aVar[aCount]='\0';                        //This is a must
//
//    return aVar;
//}

void gxBufferFileReader::Read(unsigned char& v) {
    assert(CanRead(sizeof(unsigned char)));
    memcpy((void*)&v, (void*)(fileStartPtr+filePointer), sizeof(unsigned char));
    filePointer+=sizeof(unsigned char);
}

void gxBufferFileReader::Read(bool& v) {
    assert(CanRead(sizeof(bool)));
    memcpy((void*)&v, (void*)(fileStartPtr+filePointer), sizeof(bool));
    filePointer+=sizeof(bool);
}

void gxBufferFileReader::Read(short& v) {
    assert(CanRead(sizeof(short)));
    memcpy((void*)&v, (void*)(fileStartPtr+filePointer), sizeof(short));
    filePointer+=sizeof(short);
}

void gxBufferFileReader::Read(unsigned short& v) {
    assert(CanRead(sizeof(unsigned short)));
    memcpy((void*)&v, (void*)(fileStartPtr+filePointer), sizeof(unsigned short));
    filePointer+=sizeof(unsigned short);
}

void gxBufferFileReader::Read(int& v) {
    assert(CanRead(sizeof(int)));
    memcpy((void*)&v, (void*)(fileStartPtr+filePointer), sizeof(int));
    filePointer+=sizeof(int);
}

void gxBufferFileReader::Read(unsigned int& v) {
    assert(CanRead(sizeof(unsigned int)));
    memcpy((void*)&v, (void*)(fileStartPtr+filePointer), sizeof(unsigned int));
    filePointer+=sizeof(unsigned int);
}

void gxBufferFileReader::Read(float& v) {
    assert(CanRead(sizeof(float)));
    memcpy((void*)&v, (void*)(fileStartPtr+filePointer), sizeof(float));
    filePointer+=sizeof(float);
}

void gxBufferFileReader::Read(double& v) {
    assert(CanRead(sizeof(double)));
    memcpy((void*)&v, (void*)(fileStartPtr+filePointer), sizeof(double));
    filePointer+=sizeof(double);
}

void gxBufferFileReader::Read(unsigned long& v) {
    assert(CanRead(sizeof(unsigned long)));
    memcpy((void*)&v, (void*)(fileStartPtr+filePointer), sizeof(unsigned long));
    filePointer+=sizeof(unsigned long);
}

#if !defined(__APPLE__)
void gxBufferFileReader::Read(__int64& v) {
    assert(CanRead(sizeof(__int64)));
    memcpy((void*)&v, (void*)(fileStartPtr+filePointer), sizeof(__int64));
    filePointer+=sizeof(__int64);
}
#endif

void gxBufferFileReader::ReadBuffer(unsigned char* buffer, unsigned long cnt) {
    assert(buffer);
    assert(CanRead(cnt));
    memcpy(buffer, (fileStartPtr+filePointer), cnt);
    filePointer+=cnt;
}
