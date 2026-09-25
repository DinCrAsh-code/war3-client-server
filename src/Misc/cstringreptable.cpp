//============================================================================
//  TSHashTable<CStringRep,HASHKEY_STR>'s loop-carrying members:
//  0x6F4C4C90 Ptr, 0x6F4C55A0 Initialize, 0x6F4C59E0 CheckGrow,
//  0x6F4C5600 Rehash, 0x6F4C5B40 NewNode.  Same template, same instantiation
//  mechanism and same reasoning as Render/fonthash.cpp's five.
//============================================================================
#include "cstringrep.h"
#include "tshashstr.inl"

template CStringRep* CStringRepTable::Ptr(const char*);
template void        CStringRepTable::Initialize();
template int         CStringRepTable::CheckGrow(unsigned int);
template void        CStringRepTable::Rehash(unsigned int);
template CStringRep* CStringRepTable::NewNode(unsigned int, void*, void*);
