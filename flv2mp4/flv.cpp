#include "flv.h"
#include "common.h"
#include <arpa/inet.h>
#include "utility.h"

Flv::Flv(int flvFd)
{
	mFlvFd = flvFd;
	mFirstVideoOffset=0;
	mFirstAudioOffset=0;
	mFirstDataOffset=0;
	mhasVideo = 0;
	mHasAuddio = 0;
	mAnaylyVideoTagNum = 0;
	mAnaylyAudioTagNum = 0;
	mPos = 0;
	AnalyFlvHeader();
	while( 1 )
	{
		int res = AnalyFlvTag();
		if( -1 == res || 2 == res )
			break;
	}
}

Flv::~Flv()
{
}
int Flv::AnalyFlvHeader()
{
	//analyse from begin.
	if( -1 == lseek(mFlvFd,0,SEEK_SET) )
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: lseekerror:%d error:%s",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno));
		return -1;
	}
	int size = read(mFlvFd,mTemp,9);
	if(-1 == size || 0 == size )
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: readerror:%d error:%s size:%d",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno),size);
		return -1;
	}
	mPos+=size;
	if( 0x46 != mTemp[0] || 0x4c!= mTemp[1]  || 0x56 != mTemp[2])
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: not flv file.",__FILE__,__FUNCTION__,__LINE__);
		return -1;
	}
	mhasVideo = !!( mTemp[4]&0x01 );
	mHasAuddio = !!( mTemp[4]&0x04 );
	if( 1 == (int) mTemp[3] );//do nothing.
	else 
	{
		int headsize =  ntohl(  *( (int *)( mTemp+5 ) ) );
		if( -1 == lseek(mFlvFd,headsize-9,SEEK_CUR) )//larger file.
		{
			logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: lseekerror:%d error:%s",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno));
			return -1;
		}
		mPos +=headsize-9;
	}

}


int Flv::AudioTag()
{
	int size = read(mFlvFd,mTemp,1);
	if(-1 == size || 0 == size )
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: readerror:%d error:%s size:%d",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno),size);
		return -1;
	}
	mPos += size;
	mSoundFormat = (mTemp[0] & 0xF0)/(16);
	mAnaylyAudioTagNum++;
	if( AUDIO_FORMAT_ACC == mSoundFormat && 2==mAnaylyAudioTagNum )
	{
		mFirstAudioOffset = mPos-(1+11+4);
	}
	return size;	
}

int Flv::VideoTag()
{
	int size = read(mFlvFd,mTemp,1);
	if(-1 == size || 0 == size )
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: readerror:%d error:%s size:%d",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno),size);
		return -1;
	}
	mPos += size;
	mCodecID = mTemp[0]&0x0F;
	mAnaylyVideoTagNum++;
	if( VIDEO_CODEID_H264 == mCodecID && 2 == mAnaylyVideoTagNum  )
	{
		mFirstVideoOffset = mPos-(1+11+4);
	}
	return size;
}


int Flv::AnalyFlvTag()
{
	int size = read(mFlvFd,mTemp,4);//previous tag len.
	if(-1 == size || 0 == size )
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: readerror:%d error:%s size:%d",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno),size);
		return -1;
	}
	mPos += size;
	
	int lenPrevious = ntohl( *( (int *)mTemp ) );
	size = read(mFlvFd,mTemp,11);
	if(-1 == size || 0 == size )
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: readerror:%d error:%s size:%d",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno),size);
		return -1;
	}
	mPos += size;
	char TagType = mTemp[0]&0x1F;
	int DataSize = Net3ByteToHost(mTemp+1);
	int seekSize=0;
	switch(TagType)
	{
		case 8:
			seekSize = AudioTag();
			break;
		case 9:
			seekSize = VideoTag();
			break;
		case 18://script.
		default:
			break;
	}
	if( -1 == lseek(mFlvFd,DataSize-seekSize,SEEK_CUR) )
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: lseekerror:%d error:%s",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno));
		return -1;
	}
	mPos += DataSize-seekSize;
	if( mhasVideo && mHasAuddio)
	{
		if( mAnaylyVideoTagNum>=2&&mAnaylyAudioTagNum>=2) 
		{
			mFirstDataOffset= mFirstVideoOffset<=mFirstAudioOffset ? mFirstVideoOffset:mFirstAudioOffset;
			return 2;
		}
	}
	if(mhasVideo)
	{
		if(mAnaylyVideoTagNum>=2)
		{
			mFirstDataOffset = mFirstVideoOffset;
			return 2;
		}
	}
	if(mHasAuddio)
	{
		if(mAnaylyAudioTagNum>=2)
		{
			mFirstDataOffset = mFirstAudioOffset;
			return 2;
		}
	}
	return 0;
}

int Flv::GetFirstDataOffset()
{
	return mFirstDataOffset;
}
int Flv::GetFirstVideoDataOffset()
{
	return mFirstVideoOffset;
}
int Flv::GetFirstAudioDataOffset()
{
	return mFirstAudioOffset;
}
int Flv::GetVideoTagHeaderSize()
{
	switch(mCodecID)
	{
		case VIDEO_CODEID_H264:		//h264
			return VIDEO_HEADTAGSIZE_H264;
		default:
			logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: the mCodecID is no h264. mCodecID:%d",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno),mCodecID);
		return VIDEO_HEADTAGSIZE_H264;
	}
	return 0;
}
int Flv::GetAudioTagHeaerSize()
{
	switch(mSoundFormat)
	{
		case AUDIO_FORMAT_ACC:	//aac.
			return AUDIO_HEADTAGSIZE_AAC;
		default:
			logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: the mSoundFormat is not aac mSoundFormat:%d",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno),mSoundFormat);
			return AUDIO_HEADTAGSIZE_AAC;
	}
	return 0;
}

