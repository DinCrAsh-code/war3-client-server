//============================================================================
//  TAllocatorHashTable<JASSNAMENODE,HASHKEY_STR,64>'s loop-carrying members:
//  0x6F44EB80 Ptr, 0x6F451E30 Initialize, 0x6F4536B0 CheckGrow, 0x6F451FB0
//  Rehash, 0x6F454110 NewNode.  One translation unit for all five, the way
//  Render/prefetchhash.cpp's own comment explains: none of them can inline
//  into another at /O2 (each has a loop) and the shipped code has a real
//  call at every edge between them.
//============================================================================
#include "jasssymtables.h"
#include "allochash.inl"

template JASSNAMENODE* JassNameTable::Ptr(const char*);
template void          JassNameTable::Initialize();
template int           JassNameTable::CheckGrow(unsigned int);
template void          JassNameTable::Rehash(unsigned int);
template JASSNAMENODE* JassNameTable::NewNode(unsigned int, void*, void*);
