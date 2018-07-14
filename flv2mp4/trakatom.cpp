#include "trakatom.h"
#include "common.h"


TrakAtom::TrakAtom(int fd,TrackerType trackerType,int size,char* type):mFd(fd),mTrackerType(trackerType),Atom(size,type)
{
	mStcoAtom=0;
	mStszAtom=0;
	mStscAtom=0;
	mStssAtom=0;
}
TrakAtom::~TrakAtom()
{
	SAFE_DELETE(mStcoAtom);
	SAFE_DELETE(mStszAtom);
	SAFE_DELETE(mStscAtom);
	SAFE_DELETE(mStssAtom);
}
void TrakAtom::CreateStcoAtom(int fd,int size,char* type)
{
	if( !mStcoAtom )
		mStcoAtom = new StcoAtom(fd,size,type);
	else
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: mStcoAtom has exist:%u",__FILE__,__FUNCTION__,__LINE__,mStcoAtom);
}

void TrakAtom::CreateStszAtom( int fd,int size,char* type )
{
	if( !mStszAtom )
		mStszAtom = new StszAtom(fd,size,type);
	else
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: mStszAtom has exist:%u",__FILE__,__FUNCTION__,__LINE__,mStszAtom);

}

void TrakAtom::CreateStscAtom( int fd,int size,char* type )
{
	if( !mStscAtom )
		mStscAtom = new StscAtom(fd,size,type);
	else
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: mStscAtom has exist:%u",__FILE__,__FUNCTION__,__LINE__,mStscAtom);
}

void TrakAtom::CreateStssAtom( int fd,int size,char* type )
{
	if( !mStssAtom )
		mStssAtom = new StssAtom(fd,size,type);
	else
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: mStssAtom has exist:%u",__FILE__,__FUNCTION__,__LINE__,mStssAtom);
}

void TrakAtom::ConstructOffset( OffsetMap& offsetMap )
{	//get entries number.
	int mStcoEntriesNum = mStcoAtom->GetEntriesNum();
	int mStszEntriesNum = mStszAtom->GetEntriesNum();
	int mStscEntriesNum = mStscAtom->GetEntriesNum();
	//get entries tables pointer.
	int* mStcoEntriesTable = mStcoAtom->GetEnriesTable();
	int* mStszEntriesTable = mStszAtom->GetEnriesTable();
	ScEntry* mStscEntriesTable = mStscAtom->GetEnriesTable();
	int mStcoI=0;
	int mStszI=0;
	int mStscI=0;
	int mStssI=0;
	//construct mp4 meida data offet.
	for( ; mStcoI < mStcoEntriesNum ; mStcoI++ )
	{
		int CoOffSet = mStcoEntriesTable[mStcoI];
		if( (mStscI+1) == mStscEntriesNum );//do nothing.
		else if( (mStcoI+1)==mStscEntriesTable[mStscI+1].trunkNum )//if it is next sample to trunk entry,we use it.
		{
			mStscI++;
		}
		int ScSampleNum = mStscEntriesTable[mStscI].SampeNum;
		for(int i=0;i<ScSampleNum;i++)
		{

			OffSetInfo* p = new OffSetInfo();
			memset(p,0,sizeof(OffSetInfo));
			p->dirNum = DIR_FLV;
			p->offset = CoOffSet;
			if( TRACKER_TYPE_VIDEO == mTrackerType )
				p->dataType = DATATYPE_FLV_VIDEO;
			else if( TRACKER_TYPE_SOUND == mTrackerType )
				p->dataType = DATATYPE_FLV_AUDIO;
			p->size = mStszEntriesTable[mStszI];
			offsetMap[CoOffSet] = p;
			if(  TRACKER_TYPE_VIDEO == mTrackerType ) //only video has the keyframe.
			{
				if( !mStssAtom->IsKeyFrame(mStszI) )
					p->isKey=1;
				else
					p->isKey=0;
			}
			CoOffSet += mStszEntriesTable[mStszI];			
			mStszI++;
		}
	}
	
}

