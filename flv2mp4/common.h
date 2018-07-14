//file: common.h
//author:dhlu
//date: 2011-09-26
//mail: dhlu@tudou.com
//comon define,common enum type,and common struct data type.them will be used in losts of classes.

#ifndef COMMON_H
#define COMMON_H
#include "iflv2mp4.h"
#include <map>
#include <set>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "logtxt.h"
 
#define SAFE_DELETE(x){if(x){ delete x; x=0; }}
#define SAFE_DELETE_ARRAY(x){ if(x) { delete [] x; x=0; } }

const char TEST_LOG=0;

//video compress format.
enum
{
	VIDEO_CODEID_H263 = 2,
	VIDEO_CODEID_H264 = 7	
};
enum 
{
	VIDEO_HEADTAGSIZE_H263 = 16,//need check.
	VIDEO_HEADTAGSIZE_H264 = 16
};

//sound compress format.
enum
{
	AUDIO_FORMAT_MP3 = 2,
	AUDIO_FORMAT_ACC = 10
};

enum 
{
	AUDIO_HEADTAGSIZE_MP3 = 13,//need check.
	AUDIO_HEADTAGSIZE_AAC = 13
};

//tracker type
typedef enum TrackerType_
{
	TRACKER_TYPE_OTHER = 0,
	TRACKER_TYPE_VIDEO = 1,
	TRACKER_TYPE_SOUND = 2
}TrackerType;

typedef struct sc_
{
	int trunkNum;
	int SampeNum;
}ScEntry;


//dir type
enum
{
	DIR_FLV=0,
	DIR_HEAD=1
};
//data type
enum 
{
	DATATYPE_HEAD_INFO =1,
	DATATYPE_FLV_VIDEO =2,
	DATATYPE_FLV_AUDIO =3
};
extern char gDir[2][DIR_MAX_LEN];			//all reference dir .


typedef struct OffsetInfo_
{
	char dirNum;			//dir number.
	unsigned long long offset;	//file's offset.
	char dataType;	//1,headinfo,2,video,3 audio
	char isKey; 	//if dataType=2,1,is key frame.0,no.
	int size;	//data size in the file.
}OffSetInfo;	

typedef std::map<unsigned long long,OffSetInfo*> OffsetMap;
typedef std::set<int> KyFrmTable;//key frame table.


#endif
