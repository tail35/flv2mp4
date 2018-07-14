#include "atom.h"
#include <string.h>

Atom::Atom(int size,char* type):mSize(size)
{
	memcpy(mType,type,4);
}
Atom::~Atom()
{
}

