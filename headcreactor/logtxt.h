#ifndef LOGTXT_H
#define LOGTXT_H
enum
{
LEVEL_ERROR,
LEVEL1,
LEVEL2,
LEVEL3,
LEVEL4
};

#define ERR_LOG  "error.log"


extern 	char* gDssPidLogPath;
extern char *gPrivateDir;

extern int GetFileLen(char *dir);
extern void logtxt(int level,char* format, ...);
extern void logtxt2(char* dir,int level,char* format, ...);
extern void logtxt3(char* dir,int level,char* format, ...);
extern void logFileInit();
#endif
