//file: stscatom.h
//author:dhlu
//date: 2011-09-26
//mail: dhlu@tudou.com
//read and analy stsc talbe from .head file.

#ifndef STSC_ATOM_H
#define STSC_ATOM_H
#include "atom.h"
#include "common.h"

class  StscAtom:public Atom
{
public:
	StscAtom( int fd,int size,char* type );
	~StscAtom();
	int GetEntriesNum();
	ScEntry* GetEnriesTable();
private:
	ScEntry* mScTable;
	int mEntriesnum;
	char* mTemp;
};

#endif

