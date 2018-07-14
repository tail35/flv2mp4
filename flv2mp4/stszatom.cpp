#include "stszatom.h"
#include "common.h"

StszAtom::StszAtom(int fd,int size,char* type):Atom(size,type)
{
	mSamplesSizeTable = 0;
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
	mDefaultSize= ntohl( *( (int*)(mTemp+4) ) );
	mEntriesnum = ntohl( *( (int*)(mTemp+8) ) );
	char *p = mTemp+12;
	mSamplesSizeTable = new int[mEntriesnum];
	for( int i=0; i < mEntriesnum; i++,p+=4 )
	{
		mSamplesSizeTable[i]= ntohl( *( (int*) p ) );
	}	
	int test=0;
	test++;
}
StszAtom::~StszAtom()
{
	SAFE_DELETE_ARRAY( mTemp );
	SAFE_DELETE_ARRAY( mSamplesSizeTable );
}


int StszAtom::GetEntriesNum()
{
	return mEntriesnum;
}
int* StszAtom::GetEnriesTable()
{
	return mSamplesSizeTable;
}


