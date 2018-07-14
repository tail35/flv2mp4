//file: stszatom.h
//author:dhlu
//date: 2011-09-26
//mail: dhlu@tudou.com
//read and analy stsz talbe from .head file.

#ifndef STSZ_ATOM_H
#define STSZ_ATOM_H
#include "atom.h"

class StszAtom:public Atom
{
public:
	StszAtom(int fd,int size,char* type);
	~StszAtom();
	int GetEntriesNum();
	int* GetEnriesTable();
private:
	int* mSamplesSizeTable;
	int mDefaultSize;
	int mEntriesnum;
	char* mTemp;
};

#endif

