//file: trakatom.h
//author:dhlu
//date: 2011-09-26
//mail: dhlu@tudou.com
//generate all stbl's children atom.and construct mp4 media sample offset in virtaul mp4 file.

#ifndef TRAK_ATOM_H
#define TRAK_ATOM_H

#include "common.h"
#include "stcoatom.h"
#include "stscatom.h"
#include "stssatom.h"
#include "stszatom.h"

class TrakAtom: public Atom
{
public:
	TrakAtom(int fd,TrackerType trackerType,int size,char* type);
	~TrakAtom();
public:
	void CreateStcoAtom( int fd,int size,char* type );
	void CreateStszAtom( int fd,int size,char* type );
	void CreateStscAtom( int fd,int size,char* type );
	void CreateStssAtom( int fd,int size,char* type );
	void ConstructOffset(OffsetMap& offsetMap);
private:
	TrackerType mTrackerType;
	int mFd;
	StcoAtom* mStcoAtom;
	StszAtom* mStszAtom;
	StscAtom* mStscAtom;
	StssAtom* mStssAtom;
};

#endif
