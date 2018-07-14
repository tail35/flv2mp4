//file: stcoatom.h
//author:dhlu
//date: 2011-09-26
//mail: dhlu@tudou.com
//read and analy stco talbe from .head file.
#ifndef STCO_ATOM_H
#define STCO_ATOM_H
#include "atom.h"

class StcoAtom:public Atom
{
public:
	StcoAtom(int fd,int size,char* type);
	~StcoAtom();
	int GetEntriesNum();
	int* GetEnriesTable();
private:
	int* mTrunkOffset;
	int mEntriesnum;
	char* mTemp;
};

#endif

