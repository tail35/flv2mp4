

#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <logtxt.h>

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

//return 0,BigDianOs,else littleDian os.
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
class autoClose
{
	public:
	autoClose(){}
	~autoClose()
	{
		if(-1!=fd1) close(fd1);
		if(-1!=fd2) close(fd2);
	}
	void setfd1(int infd1){fd1=infd1;}
	void setfd2(int infd2){fd2=infd2;}
	int fd1,fd2;
};

int HeadCreactor(char* path)
{
	autoClose autc;
	char temp[1024]={0};
	AddSuffix(path,temp,sizeof(temp),".head");
	if(-1 == GetFileLen(path) )
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: ",__FILE__,__FUNCTION__,__LINE__);
		return -1;
	}
	int fd1 = open(path,O_RDONLY);
	autc.setfd1(fd1);
	if(-1 == fd1)
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: openFailed,path:%s errno:%d errnos:%s",__FILE__,__FUNCTION__,__LINE__,path,errno,strerror(errno));
		return 0;
	}

	int fd2 = open(temp,O_CREAT | O_TRUNC | O_RDWR);
	autc.setfd1(fd2);
	if(-1 == fd2)
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: openFailed,temp:%s errno:%d errnos:%s",__FILE__,__FUNCTION__,__LINE__,temp,errno,strerror(errno));
		return 0;
	}

	int size = 0;
	unsigned long long atomLen=0;
	while(1)
	{
		size = read(fd1,temp,8);
		if(0 == size ) return 0;//end of the file.
		if(-1 == size) 
		{
			logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: read big size error fd1:%d errno:%d errnos:%s",__FILE__,__FUNCTION__,__LINE__,fd1,errno,strerror(errno));
			return -1;
		}
		temp[8]=0;
		atomLen = ntohl(  *( (int*)temp ) );
		switch(atomLen)
		{
			case 0:
				{
					logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: end atom",__FILE__,__FUNCTION__,__LINE__);
					return 0;
				}
			case 1:
				{
					size = read(fd1,temp+size,8);
					if( 8 != size || 0==size)
					{
						logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: read big size error fd1:%d errno:%d errnos:%s",__FILE__,__FUNCTION__,__LINE__,fd1,errno,strerror(errno));
						return -1;
					}
					atomLen == ntohunsignedll(temp+size);
					if( -1 == write(fd1,temp,16) )
					{
						logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: write failed fd1:%d errno:%d errnos:%s",__FILE__,__FUNCTION__,__LINE__,fd1,errno,strerror(errno));
						return -1;
					}
					
					if(-1==lseek64(fd1,atomLen-16,SEEK_CUR) )//seek to next atom begin.
					{
						logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: lseek64 failed fd1:%d errno:%d errnos:%s",__FILE__,__FUNCTION__,__LINE__,fd1,errno,strerror(errno));
						return 0;
					}	
					break;
				}
			default:
				{
					if( -1 == write(fd2,temp,8) )
					{
						logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: write failed fd1:%d errno:%d errnos:%s",__FILE__,__FUNCTION__,__LINE__,fd1,errno,strerror(errno));
						return -1;
					}
					if(8 == atomLen) break;
					if(0 == strncmp(temp+4,"mdat",4) )
					{
						if(-1==lseek64(fd1,atomLen-8,SEEK_CUR) )//seek to next atom begin.
						{
							logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: lseek64 failed fd1:%d errno:%d errnos:%s",__FILE__,__FUNCTION__,__LINE__,fd1,errno,strerror(errno));
							return 0;
						}
						break;//we need't write data to the file.
					}
					int writeLen=8;
					int readsize = atomLen-8;
					while(1)
					{
						size = read(fd1,temp, sizeof(temp) <= readsize ? sizeof(temp) : readsize);
						if( !size ) break;
						if( -1 == size || 0==size )
						{
							logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: read failed fd1:%d errno:%d errnos:%s",__FILE__,__FUNCTION__,__LINE__,fd1,errno,strerror(errno));
							return -1;
						}
						int wsize = write(fd2,temp,size);
						if( -1 == wsize || wsize != size)
						{
							logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: write failed fd1:%d errno:%d errnos:%s",__FILE__,__FUNCTION__,__LINE__,fd1,errno,strerror(errno));
							return -1;
						}
						writeLen+=size;
						readsize-=size;
						if(writeLen == atomLen || 0 == readsize)
							break;
						if(writeLen > atomLen)
						{
							logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: write more data:writeLen:%d atomLen:%d",__FILE__,__FUNCTION__,__LINE__,writeLen,atomLen);
							return -1;
						}
					}//end default while(1)
				}//end default
			
		}//end switch(atomLen)
	}//end while(1)
	return true;
}

void usage()
{
	printf("creator [mp4 file  name] ");
}

int main(int argc,char** argv)
{
	if(argc != 2 )
	{
		usage();
		return -1;
	}
	if(0!=access(argv[1],F_OK))
	{
		printf("file %s is no exist!\r\n",argv[1]);
		return -1;
	}
	if(-1 == HeadCreactor( argv[1] ) )
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: CreateHeadFailed!",__FILE__,__FUNCTION__,__LINE__);
	}

	return 0;
}
