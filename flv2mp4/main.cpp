#include <stdio.h>
#include <iostream>
#include "iflv2mp4.h"
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void usage()
{
	printf("usage:Flv2Mp4 [f4v filename] [output mp4 name]\r\n");
	printf("Flv2Mp4 1.f4v\r\n");
}

int main(int argc,char**argv)
{	
	if(argc != 3 )
	{
		usage();
		return 0;
	}
	if(0!=access(argv[1],F_OK))
	{
		printf("file %s is no exist!\r\n",argv[1]);
		return 0;
	}

	IFlv2Mp4 *flv2Mp4 = CreateIFlv2Mp4Object();
	int fd1 = flv2Mp4->Flv2Mp4Open( argv[1] ,O_RDONLY);
	int fd2 = open( argv[2],  O_CREAT | O_WRONLY | O_TRUNC );
	if( -1 == fd1 || -1 == fd2 )
	{
		perror("open failed");
		return -1;
	}
	char temp[1024]={0};
	printf( "fileLen:%lld\r\n",flv2Mp4->Flv2Mp4GetFileLen(fd1) );
	while(1)
	{
		int size = flv2Mp4->Flv2Mp4Read(fd1,temp,sizeof(temp));
		if(0 == size)
		{
			printf("Complete successfully.\r\n");
			break;
		}
		if(-1 == size)
		{
			perror("read error:");
			return -1;
		}
		size = write(fd2,temp,size);
		if(-1 == size)
		{
			perror("write error:");
			return -1;
		}
		
	}
	close(fd2);
	return 0;
}
