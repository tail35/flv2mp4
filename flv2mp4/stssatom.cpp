#include "stssatom.h"
#include "common.h"

StssAtom::StssAtom(int fd,int size,char* type):Atom(size,type)
{
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
	for( int i=0; i < mEntriesnum; i++,p+=4 )
	{
		mSsTable.insert( ntohl( *( (int*) p ) ) );
	}
}

StssAtom::~StssAtom()
{
	SAFE_DELETE_ARRAY( mTemp );
}


int StssAtom::GetEntriesNum()
{
	return mEntriesnum;
}
//return value are temporarily.please don't save and use it at other place..
KyFrmTable& StssAtom::GetEnriesTable()
{
	return mSsTable;
}

//-1,is not key,0,it is key.
int StssAtom::IsKeyFrame( int SampleNum )
{
	if( mSsTable.find( SampleNum ) == mSsTable.end())
		return -1;
	else
		return 0;
}



