//file: flv2mp4.h
//author:dhlu
//date: 2011-09-26
//mail: dhlu@tudou.com
//class Flv2Mp4 are main class that implement constructing virtual mp4 from .f4v and .head file which already existed.

#ifndef FLV2MP4_H
#define FLV2MP4_H
#include "iflv2mp4.h"
#include <map>
#include "trakatom.h"
#include "common.h"

class Flv2Mp4:public IFlv2Mp4
{	
public: 
    Flv2Mp4();
    virtual ~Flv2Mp4();
public:
    int Flv2Mp4Open(const char *pathname,int flags, int mode=0);
    int Flv2Mp4Seek(int fd, int offset, int whence);
    int Flv2Mp4Read(int fd, char* buf, int count);
    int Flv2Mp4Close(int fd);
    unsigned long long Flv2Mp4GetFileLen(int fd);
private:
	int ConstructDir(char* fileName);
	int ConstructHeadOffset();
	int ConstructFlvOffset();
	int IsWeCareAtom(char* p);
	int AnalyHDLRAtom(int fd,int atomLen,TrackerType& isVideo);
	int ReadAndConstructMap(int fd,int& fileEnd,TrackerType& isVideo,char* pType);	
	void TestMp4Offset();
private:
	OffsetMap mOffsetMap;
	//
	int mHeadFd;
	int mFlvFd;
	unsigned long long mFileLen;
private:
	//atom
	TrakAtom* mArrayTrakAtom[3]; //0,other,1,video,2,audio.
private:
	//for read
	int mPos;
	OffsetMap::iterator mIt;
};

#endif
