//file: stssatom.h
//author:dhlu
//date: 2011-09-26
//mail: dhlu@tudou.com
//read and analy stss talbe from .head file.

#ifndef STSS_ATOM_H
#define STSS_ATOM_H
#include "atom.h"
#include "set"
#include "common.h"

class StssAtom:public Atom
{
public:
	StssAtom(int fd,int size,char* type);
	~StssAtom();
	int GetEntriesNum();
	KyFrmTable& GetEnriesTable();

	int IsKeyFrame( int SampleNum );
private:
	char* mTemp;
	int mEntriesnum;
	KyFrmTable mSsTable;
};

#endif

