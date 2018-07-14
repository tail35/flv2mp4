//file: iflv2mp4.h
//author:dhlu
//date: 2011-09-26
//mail: dhlu@tudou.com
//this file is the interface that construct mp4 file from flv and mp4head which has existed.

#ifndef IFLV2MP4_H
#define IFLV2MP4_H
#define DIR_MAX_LEN 512  //max bytes of all full name of file must be letter then 512.you can set it bigger ,if you need.

class IFlv2Mp4
{
public:
	IFlv2Mp4(){};
	virtual ~IFlv2Mp4(){};
public:
	virtual int Flv2Mp4Open(const char *pathname,int flags, int mode=0)=0;
	virtual int Flv2Mp4Seek(int fd, int offset, int whence)=0;
	virtual int Flv2Mp4Read(int fd, char *buf, int count)=0;
	virtual int Flv2Mp4Close( int fd )=0;
	virtual unsigned long long Flv2Mp4GetFileLen(int fd)=0;
};
//IFlv2Mp4 need delete by your selft.
IFlv2Mp4* CreateIFlv2Mp4Object();


#endif 
