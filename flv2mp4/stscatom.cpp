#include "stscatom.h"
#include "common.h"

StscAtom::StscAtom( int fd,int size,char* type ):Atom(size,type)
{
	mScTable = 0;
	size-=8;
	mTemp = new char[size];		
	size = read(fd,mTemp,size);
	if( 0 == size ) 
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: this case should not be 0:%d error:%s",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno));
		return;
	}
	if( -1 == size) 
	{
		logtxt3(ERR_LOG,LEVEL_ERROR,"%s %s %d: readerror:%d error:%s",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno));
		return;//read error.
	}
	mEntriesnum = ntohl( *( (int*)(mTemp+4) ) );
	char *p = mTemp+8;
	mScTable = new ScEntry[mEntriesnum];
	for( int i=0; i < mEntriesnum; i++,p+=12 )
	{
		mScTable[i].trunkNum = ntohl( *( (int*) (p) ) );
		mScTable[i].SampeNum = ntohl( *( (int*) (p+4) ) );	
	}
}
StscAtom::~StscAtom()
{
	SAFE_DELETE_ARRAY( mTemp );
	SAFE_DELETE_ARRAY( mScTable );
}

int StscAtom::GetEntriesNum()
{
	return mEntriesnum;
}
ScEntry* StscAtom::GetEnriesTable()
{
	return mScTable;
}


