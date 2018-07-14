//class: Atom
//author:dhlu
//date: 2011-09-26
//mail: dhlu@tudou.com
//this class is the base atom of mp4.

#ifndef ATOM_H
#define ATOM_H

class Atom
{
public:
	Atom(int size,char* type);
	virtual ~Atom();
private:
	int mSize;
	char mType[4];
};


#endif

