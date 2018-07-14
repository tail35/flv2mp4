#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include "logtxt.h"
#include <string.h>
#include <errno.h>

#define LOG_DIR "/log.txt"
#define LOGFILENAME "logfile.ini"

FILE* pFile=0;
char temp[1<<20];
char tmpbuf[1<<20]={0};
char usecond[32]={0};


char* gPrivateLogDir="" ;
char *gPrivateDir="";


void GetFormatTime()
{	
	time_t lt1; 
	struct 	timeval tv;
	time( &lt1 ); 
	struct tm *newtime=localtime(&lt1); 
	gettimeofday(&tv,NULL);	

	strftime( tmpbuf, 128, "%Y-%m-%d %H:%M:%S ", newtime); 
	//snprintf(usecond,sizeof(usecond),"se:%d use:%d",tv.tv_sec,tv.tv_usec);
	//strncat(tmpbuf,usecond,sizeof(tmpbuf) - strlen(tmpbuf) );

}
//return -1 indicate the file is not exist.
int GetFileLen(char *dir)
{
	//modify by zcp ,because the ftell need lseek(SEEK_END),ftell.
	struct stat st;
	if(stat(dir,&st)!=0)
	{
		printf("stat error:%s\n",strerror(errno));
		return -1;
	}
	return st.st_size;

}
void logtxt(int level,char* format, ...)
{
    if( LEVEL2 != level )
       return;
    if( !pFile )
    {
      pFile = fopen(LOG_DIR,"a");
      if(!pFile)
        return ;
    }

    int fileLen = GetFileLen(LOG_DIR);
    if(-1 == fileLen)
    {	
	fclose(pFile);
	pFile = fopen(LOG_DIR,"a");
        if(!pFile)
          return;
        fileLen = ftell(pFile);
    }
    if( fileLen > (1<<20) )
    {
      fclose(pFile);
      remove(LOG_DIR);
      pFile = fopen(LOG_DIR,"a");
      if(!pFile)
         return;
    }

    va_list ap;
    va_start(ap,format);
    memset(temp,0,sizeof(temp));
    GetFormatTime();
    snprintf(temp,sizeof(temp)/sizeof(char),tmpbuf);
    char *p = temp + strlen(temp);
    char len = sizeof(temp) - strlen(temp);
    vsnprintf(p,len,format,ap);	
    va_end(ap);
    p = temp + strlen(temp);
    len = sizeof(temp) - strlen(temp);
    snprintf(p,len,"\r\n");
    fwrite(temp,strlen(temp),sizeof(char),pFile);
    fflush(pFile);
}
//follow function you can set the dir

char temp2[1<<20]={0};
char tempbuf2[1<<20]={0};

char file2[256]={0};
void logtxt2(char* dir,int level,char* format, ...)
{
    if(0 == dir || LEVEL2 != level)
	return;
    FILE* pFile=0;
	
	memset(file2,0,sizeof(file2));
	snprintf(file2,sizeof(file2),"%s%s",gPrivateLogDir,dir);
     pFile = fopen(file2,"a");
      if(!pFile)
      {
      	char *p = strerror(errno);
		printf("openlogfailed:%s\r\n",p);
        return ;
      }
    int fileLen = GetFileLen(file2);
    if(-1 == fileLen)
    {
        fclose(pFile);
        pFile = fopen(file2,"a");
        if(!pFile)
          return;
        fileLen = ftell(pFile);
    }
    if( fileLen > (1<<20) )
    {
      fclose(pFile);
      remove(file2);
      pFile = fopen(file2,"a");
      if(!pFile)
         return;
    }
    //get time        
    time_t lt1;
    time( &lt1 );
    struct tm *newtime=localtime(&lt1);
    strftime( tempbuf2, 128, "%Y-%m-%d %H:%M:%S ", newtime);
    //end get time;

    va_list ap;
    va_start(ap,format);
    memset(temp2,0,sizeof(temp2));
    snprintf(temp2,sizeof(temp2)/sizeof(char),tempbuf2);
    char *p = temp2 + strlen(temp2);
    char len = sizeof(temp2) - strlen(temp2);
    vsnprintf(p,len,format,ap);
    va_end(ap);
    p = temp2 + strlen(temp2);
    len = sizeof(temp2) - strlen(temp2);
    snprintf(p,len,"\r\n");
	fwrite(temp2,strlen(temp2),sizeof(char),pFile);
    fflush(pFile);
    fclose(pFile);
}

/*
 the function different the above is which can rolling the file when get the limit.
*/
char temp3[1<<20]={0};
char tempbuf3[1<<20]={0};

static int logfileid=0;
char file3[256]={0};
void logtxt3(char* dir,int level,char* format, ...)
{
    if(0 == dir || LEVEL_ERROR != level) return;
    FILE* pFile=0;

	memset( file3,0,sizeof(file3) );
	snprintf(file3,sizeof(file3),"%s%s.%d",gPrivateLogDir,dir,logfileid);
     pFile = fopen(file3,"a");
      if(!pFile)
      {
      	char *p = strerror(errno);
		printf("openlogfailed:%s\r\n",p);
        return ;
      }
    int fileLen = GetFileLen(file3);
    if(-1 == fileLen)
    {
        fclose(pFile);
        pFile = fopen(file3,"a");
        if(!pFile)
          return;
        fileLen = ftell(pFile);
    }
    if( fileLen > (1<<20) )
    {
      fclose(pFile);     
      snprintf(file3,sizeof(file3),"%s%s.%d",gPrivateLogDir,dir,++logfileid);
      pFile = fopen(file3,"a");
      if(!pFile)
         return;
    }
    //get time        
    time_t lt1;
    time( &lt1 );
    struct tm *newtime=localtime(&lt1);
    strftime( tempbuf3, 128, "%Y-%m-%d %H:%M:%S ", newtime);
    //end get time;

    va_list ap;
    va_start(ap,format);
    memset(temp3,0,sizeof(temp3));
    snprintf(temp3,sizeof(temp3)/sizeof(char),tempbuf3);
    char *p = temp3 + strlen(temp3);
    char len = sizeof(temp3) - strlen(temp3);
    vsnprintf(p,len,format,ap);
    p = temp3 + strlen(temp3);
    len = sizeof(temp3) - strlen(temp3);
    snprintf(p,len,"\r\n");
	fwrite(temp3,strlen(temp3),sizeof(char),pFile);
    va_end(ap);
    fflush(pFile);
    fclose(pFile);
}

