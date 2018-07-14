//file: flv2mp4.h
//author:dhlu
//date: 2011-09-26
//mail: dhlu@tudou.com
//we just use logtxt3 function in this project.use it to record logs.
#ifndef LOGTXT_H
#define LOGTXT_H
#include <errno.h>

enum
{
LEVEL_ERROR,
LEVEL1,
LEVEL2,
LEVEL3,
LEVEL4
};

#define ERR_LOG  "error.log"
#define MP4_OFFSET_video "mp4offsetvideo.log"
#define MP4_OFFSET_audio "mp4offsetaudio.log"
#define FLV_DATA_OFFSET "flvdataoffset.log"

extern 	char* gDssPidLogPath;
extern char *gPrivateDir;

extern int GetFileLen(char *dir);
extern void logtxt(int level,char* format, ...);
extern void logtxt2(char* dir,int level,char* format, ...);
extern void logtxt3(char* dir,int level,char* format, ...);
extern void logFileInit();
#endif
