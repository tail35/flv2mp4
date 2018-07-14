#include "flv2mp4.h"
#include "utility.h"
#include "common.h"
#include "flv.h"


Flv2Mp4::Flv2Mp4()
{
	memset(mArrayTrakAtom,0,sizeof(mArrayTrakAtom));
	mFlvFd=-1;
	mHeadFd=-1;
	mPos=0;
	mFileLen = 0;
}

Flv2Mp4::~Flv2Mp4()
{
	
}

int Flv2Mp4::ConstructDir(char* fileName)
{
	//construct dir.
	int lenDir = strlen(fileName);
	if( lenDir >= ( DIR_MAX_LEN-sizeof(".mp4.head") ) )
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: fileName too long len:%d %s",__FILE__,__FUNCTION__,__LINE__,strlen(fileName),fileName);
		return -1;
	}
	strncpy(gDir[DIR_FLV],fileName,lenDir);
	char  fileDir[DIR_MAX_LEN]={0};
	GetFileDirAndName(fileName,fileDir,DIR_MAX_LEN);
	snprintf(gDir[DIR_HEAD],DIR_MAX_LEN,"%s.mp4.head",fileDir);
	//read head info.and construct offsetMap.
	return 0;
}

int Flv2Mp4::ConstructHeadOffset()
{
	char temp[1024]={0};
	int size=0;
	unsigned long long atomLen=0;
	int pos=0;
	//seek the mHeadFd to begin.
	if( -1 == lseek(mHeadFd,0,SEEK_SET) )
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: lseekerror:%d error:%s",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno));
		return -1;
	}
	int mp4Offet=0;
	while(1)
	{
		size = read(mHeadFd,temp,8);
		pos+=8;//recore where has read.
		if( 0 == size ) break;	//end of the file.		
		if( -1 == size)
		{
			logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: readerror:%d error:%s",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno));
			return -1;//read error.
		}
		char* type = temp+4;
		atomLen = ntohl(  *( (int*)temp ) );
		OffSetInfo* p = new OffSetInfo();
		p->dirNum = DIR_HEAD;
		p->offset = pos-8;
		p->dataType = DATATYPE_HEAD_INFO;			
		
		if(0==strncmp( type,"mdat",4))
		{	//mdat atom don't has the real data.
			if(1 == atomLen)
				p->size = 16;
			else p->size = 8;			 
		}
		else
		{
			p->size = atomLen;
		}
		mOffsetMap[mp4Offet] = p;	
		mFileLen += p->size;
		if( -1 == lseek(mHeadFd,p->size-8,SEEK_CUR) )
		{
			logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: lseekerror:%d error:%s",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno));
			return -1;
		}
		pos+=p->size-8;
		mp4Offet += atomLen;
	}
	return 0;	
}

unsigned long long Flv2Mp4::Flv2Mp4GetFileLen(int fd)
{
	return mFileLen;
}


int Flv2Mp4::AnalyHDLRAtom(int fd,int atomLen,TrackerType& isVideo)
{
	char temp[12]={0};
	int size = read(fd,temp,12);
	if( 0 == size ) 
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: this case should not be 0:%d error:%s",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno));
		return -1;
	}
	if( -1 == size) 
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: readerror:%d error:%s",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno));
		return -1;//read error.
	}
	char* type=temp+4;
	//if(0 == strncmp(type,"mhlr",4) ) some times it's 0 value.so we can't use it to judge.	
	if( 0 == strncmp(type+4,"vide",4) )
		isVideo = TRACKER_TYPE_VIDEO;//video tracker.
	else if( 0 == strncmp(type+4,"soun",4) )
		isVideo = TRACKER_TYPE_SOUND;//sound tracker.
	else
	{
		isVideo = TRACKER_TYPE_OTHER;
	}
	if( -1 == lseek(fd,atomLen-8-12,SEEK_CUR) )
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: lseekerror:%d error:%s",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno));
		return -1;
	}
	return 0;
}

int Flv2Mp4::IsWeCareAtom(char* p)
{	//the atom that we don't want recursive,we call it leaf atom.
	static char notLeafAtom[][5]={"moov","trak","mdia","hdlr","minf","stbl","stss","stsz","stsc","stco"};
	int num = sizeof(notLeafAtom)/5;
	for( int i=0;i<num;i++ )
	{
		if( 0 == strncmp(p,notLeafAtom[i],4) ) return 0;//it's  atom that we care;
	}
	return -1;//atom that we don't care.
}

int Flv2Mp4::ReadAndConstructMap(int fd,int& fileEnd,TrackerType& isVideo,char* pType)
{
	int size=0;
	char temp[16]={0};
	unsigned long long atomLen=0;
	if(fileEnd) return -1;
	size = read(fd,temp,8);
	if( 0 == size ) 
	{
		fileEnd=1;	//end of the file.		
		return -1;
	}
	if( -1 == size) 
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: readerror:%d error:%s",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno));
		return -1;//read error.
	}
	char* type=temp+4;
	atomLen = ntohl(  *( (int*)temp ) );
	
	if( 0!=IsWeCareAtom( type ) )
	{	//leaf atom (or atom that we don't want to deal)
		int seekNum = atomLen-8;
		if( 0==strncmp(type,"mdat",4) )
		{	//mdat atom don't has the real data.so only seek the size and type size.
			if( 1==atomLen )
				seekNum=16-8;
			else seekNum = 8-8;
		}
		if( -1 == lseek(fd,seekNum,SEEK_CUR) )
		{
			logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: lseekerror:%d error:%s",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno));
			fileEnd = 1;//fatal error.need to exit the recursive.
			return -1;
		}
		ReadAndConstructMap(fd,fileEnd,isVideo,pType);//pType must be parent next atom.
	}
	else
	{
		//deal hdlr atom.
		if( 0 == strncmp(type,"hdlr",4) )
		{
			if(0 == strncmp(pType,"mdia",4) )
			{
				if( -1 == AnalyHDLRAtom(fd,atomLen,isVideo) )
				{
					fileEnd = 1;
					logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: AnalyHDLRAtomerror:%d error:%s",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno));
					return -1;
				}
				//create class TrakAtom.
				mArrayTrakAtom[isVideo] = new TrakAtom(fd,isVideo,0,"trak");
			}
			else
			{
				if( -1 == lseek(fd,atomLen-8,SEEK_CUR) )
				{
					logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: lseekerror:%d error:%s",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno));
					fileEnd = 1;//fatal error.need to exit the recursive.
					return -1;
				}
			}
		}
		else if( 0 == strncmp(type,"stss",4) )
		{
			mArrayTrakAtom[isVideo]->CreateStssAtom(fd,atomLen,type);
		}
		else if( 0 == strncmp(type,"stsz",4) )
		{
			mArrayTrakAtom[isVideo]->CreateStszAtom(fd,atomLen,type);
		}
		else if( 0 == strncmp(type,"stsc",4) )
		{
			mArrayTrakAtom[isVideo]->CreateStscAtom(fd,atomLen,type);
		}
		else if( 0 == strncmp(type,"stco",4) )
		{
			mArrayTrakAtom[isVideo]->CreateStcoAtom(fd,atomLen,type);
		}
		else
			pType=type;//next atom must be children atom.
		ReadAndConstructMap(fd,fileEnd,isVideo,pType);
	}
	return 0;
}

void Flv2Mp4::TestMp4Offset()
{
	for(OffsetMap::iterator it=mOffsetMap.begin();it!=mOffsetMap.end();it++)
	{
		unsigned long long offset1 = it->first;
		OffSetInfo* p = (OffSetInfo*) it->second;
		if( DATATYPE_FLV_VIDEO == p->dataType )
			logtxt3(MP4_OFFSET_video,LEVEL_ERROR,"%d\t%d\t%d",offset1,p->size,p->isKey);
		else
			logtxt3(MP4_OFFSET_audio,LEVEL_ERROR,"%d\t%d\t%d",offset1,p->size,p->isKey);
	}
}

int Flv2Mp4::ConstructFlvOffset()
{
	int fileEnd = 0;
	TrackerType isVideo=TRACKER_TYPE_OTHER;//when we recursive the atom.we need  to record whehre the current track is video track.
	char pType[4]={0};
	ReadAndConstructMap(mHeadFd,fileEnd,isVideo,pType);	
	if(mArrayTrakAtom[TRACKER_TYPE_VIDEO])
		mArrayTrakAtom[TRACKER_TYPE_VIDEO]->ConstructOffset(mOffsetMap);
	if(mArrayTrakAtom[TRACKER_TYPE_SOUND])
		mArrayTrakAtom[TRACKER_TYPE_SOUND]->ConstructOffset(mOffsetMap);
	if( TEST_LOG )
		TestMp4Offset();
	Flv *pFlv  = new Flv(mFlvFd);
	int flvOffset = pFlv->GetFirstDataOffset();
	for(OffsetMap::iterator it=mOffsetMap.begin();it!=mOffsetMap.end();it++)
	{
		int mp4Offset = it->first;
		OffSetInfo* p = (OffSetInfo* )it->second;			
		flvOffset+=4;//previous tag len.
		if( TEST_LOG )
			logtxt3(FLV_DATA_OFFSET,LEVEL_ERROR,"%s %s %d: %d\t%d\t%d",__FILE__,__FUNCTION__,__LINE__,mp4Offset,flvOffset,p->size);
		if( DATATYPE_FLV_VIDEO == p->dataType )
			flvOffset+= pFlv->GetVideoTagHeaderSize();
		else if( DATATYPE_FLV_AUDIO == p->dataType )
			flvOffset+= pFlv->GetAudioTagHeaerSize();
		else
		{
			SAFE_DELETE(pFlv);
			logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: should not be this data type:%d ",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno),p->dataType);
			return -1;
		}
		p->offset = flvOffset;
		flvOffset+=p->size;
		mFileLen+=p->size;
	}	
	SAFE_DELETE(pFlv);
	return 0;
}

int Flv2Mp4::Flv2Mp4Open(const char *pathname,int flags, int mode)
{	
	if( -1==ConstructDir((char*)pathname) ) return -1;
		
	mHeadFd = open(gDir[DIR_HEAD],O_RDONLY);
	if(-1 == mHeadFd)
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: openFailed:%d error:%s gDir:%s",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno),gDir[DIR_HEAD]);
		return -1;
	}
	mFlvFd = open(gDir[DIR_FLV],O_RDONLY);
	if(-1 == mFlvFd)
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: openFailed:%d error:%s gDir:%s",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno),gDir[DIR_FLV]);
		return -1;
	}

	if( -1==ConstructFlvOffset() ) return -1;//must construct before HeadOffset.
	if( -1==ConstructHeadOffset() )	return -1;

	if( TEST_LOG )
		TestMp4Offset();
	mIt = mOffsetMap.begin();
	return mFlvFd;
}
int Flv2Mp4::Flv2Mp4Seek(int fd, int offset, int whence)
{
	switch(whence)
	{
		case SEEK_SET:
			mPos = offset;
			break;
		case SEEK_CUR:		
			mPos+=offset;
			break;
		case SEEK_END:
			mPos = mFileLen + offset;
			break;
		default:
			break;
	}
	if( mPos < 0 || mPos > mFileLen)
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: offset error offset:%d mPos:%d",__FILE__,__FUNCTION__,__LINE__,offset,mPos);
		return -1;
	}
	OffsetMap::iterator  it1,it2;
	it1=it2=mOffsetMap.begin();
	it2++;
	for(;it2!=mOffsetMap.end();it1++,it2++)
	{
		unsigned long long offset1 = it1->first;
		unsigned long long offset2 = it2->first;
		if( mPos>=offset1 && mPos<offset2 ) break;
	}
	mIt=it1;
	return 0;
}
int Flv2Mp4::Flv2Mp4Read(int fd, char* buf, int count)
{
	if(!buf || !count) return 0;
	int leftReadNum = mPos+count <= mFileLen ? count:(mFileLen-mPos);
	int total = leftReadNum;
	char* pb = buf;
	if( mIt==mOffsetMap.end())
	{
		return 0;//end of the file.
	}
	for(;mIt!=mOffsetMap.end()&&leftReadNum;)
	{
		unsigned long long offset1 = mIt->first;
		OffSetInfo* p = (OffSetInfo*) mIt->second;

		int hasRead = mPos-offset1;
		if( hasRead >= p->size )
		{
			logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: more data!",__FILE__,__FUNCTION__,__LINE__);
			return -1;
		}
		int fdt=0;
		if(DIR_FLV == p->dirNum)
			fdt = mFlvFd;
		else fdt = mHeadFd;
		if( -1 == lseek(fdt,p->offset+hasRead,SEEK_SET) )
		{
			logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: lseekerror:%d error:%s",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno));
			return -1;
		}
		int rn = (leftReadNum <= p->size-hasRead ) ? leftReadNum:( p->size-hasRead );
		int size = read(fdt,pb,rn);
		if( 0 == size && rn ) 
		{
			return 0;//end of the file.
		}
		if(!size&&!rn)
		{	//rn should not be zeror.must occure some error.
			logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: this case should not be 0:%d error:%s rn:%d",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno),rn);
			return -1;
		}
		if( -1 == size) 
		{
			logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: readerror:%d error:%s",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno));
			return -1;//read error.
		}		

		if(mPos>=offset1)
		{
			if( ( hasRead + size) == p->size ) mIt++;//has read one sample.should next.
		}
		
		mPos+=size;
		leftReadNum-=size;
		pb+=size;
		
	}	
	return total;
}
int Flv2Mp4::Flv2Mp4Close(int fd)
{
	if( -1!=mFlvFd )
	{
		close(mFlvFd);
		mFlvFd = -1;
	}	
	if( -1!=mHeadFd )
	{
		close( mHeadFd );
		mHeadFd = -1 ;
	}
	for(OffsetMap::iterator it=mOffsetMap.begin();it!=mOffsetMap.end();it++)
	{
		OffSetInfo* p = it->second;
		SAFE_DELETE(p);
	}		
	return 0;
}

IFlv2Mp4* CreateIFlv2Mp4Object()
{
	return new Flv2Mp4();
}

