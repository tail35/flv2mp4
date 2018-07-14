//file: utility.h
//author:dhlu
//date: 2011-09-26
//mail: dhlu@tudou.com
//some utility function.

#ifndef UTILITY_H
#define UTILITY_H


int AddSuffix(char* in,char* out,int outsize,char* suffix);
int IsBigDianOS();
unsigned long long  ntohunsignedll(char* temp);
int Net3ByteToHost(char *p,int len=3);
int GetFileName(char* path,char* out,int size);
int GetFileDirAndName(char* path,char* out,int size);

#endif

