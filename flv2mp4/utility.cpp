#include "utility.h"
#include "logtxt.h"
#include "stdio.h"
#include <arpa/inet.h>
#include <string.h>

int AddSuffix(char* in,char* out,int outsize,char* suffix)
{
	if(!in || !out || !outsize || !suffix || ( outsize<=strlen(in)+sizeof(suffix) ) )
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: in:%d out:%d outsize:%d suffix:%d",__FILE__,__FUNCTION__,__LINE__,in,out,outsize,suffix);
		return -1;
	}
	int len = strlen(in);
	snprintf(out,outsize,"%s%s",in,suffix);
	return 0;
}

int IsBigDianOS()
{
	short value=0x0102;
	short* p = &value;
	if(0x01 == p[0] )
		return 0;
	return -1;
}

unsigned long long  ntohunsignedll(char* temp)
{
	if(!temp)
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: temp Null!",__FILE__,__FUNCTION__,__LINE__);
		return 0;
	}
	if(0 == IsBigDianOS() )
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: BigDianos.Needn't transfer.",__FILE__,__FUNCTION__,__LINE__);
	}
	char p;int i=0;int j=3;
	for(;i<3;i++,j--)
	{
		p = temp[i];
		temp[i]=temp[j];
		temp[j] = p ;
	}
	return ( *(unsigned long long*)temp );
}

//len must be three
int Net3ByteToHost(char *p,int len)
{
	if( 3 != len)
		return 0;
	char temp[4]={0};	
	memcpy(temp+1,p,3);

	return ntohl( *( (int *)temp ) );
}


int GetFileName(char* path,char* out,int size)
{
	if(!path || !out )
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s%s%d: path:%d out:%d",__FILE__,__FUNCTION__,__LINE__,path,out);
		return -1;
	}
	int len = strlen(path);
	if(len <sizeof(".f4v") ) 
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s%s%d: len:%d",__FILE__,__FUNCTION__,__LINE__,len);
		return -1;
	}
	char *p1=path+len-5;char *p2=path+len-5;
	for(;p1 !=path-1 ;p1--)
	{
		if('/' == *p1) break;
	}
	strncpy(out,p1+1,size < (p2-p1) ? size:(p2-p1));
	return 0;
}

int GetFileDirAndName(char* path,char* out,int size)
{
	if(!path || !out )
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s%s%d: path:%d out:%d",__FILE__,__FUNCTION__,__LINE__,path,out);
		return -1;
	}
	int len = strlen(path);
	if(len <sizeof(".f4v") ) 
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s%s%d: len:%d",__FILE__,__FUNCTION__,__LINE__,len);
		return -1;
	}
	snprintf(out,len-4+1,"%s",path);	
	return 0;
}



