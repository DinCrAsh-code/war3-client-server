//============================================================================
//  TSHashTable<FONTHASHOBJ,HASHKEY_STR>'s five loop-carrying members:
//  0x6F4CD050 Ptr, 0x6F4CDA70 Initialize, 0x6F4CDCA0 CheckGrow,
//  0x6F4CDAD0 Rehash, 0x6F4CDD40 NewNode.
//
//  The class name is the shipped one - 0x6F4CD5F0, the table's constructor,
//  stamps `??_7?$TSHashTable@UFONTHASHOBJ@@VHASHKEY_STR@@@@6B@` - so this is
//  storm.h's own TSHashTable at a string key rather than a fourth copy of
//  the algorithm.  The bucket link sits at +0x0C rather than +0x04 because
//  FONTHASHOBJ carries a TRefCnt base, which is what the `push 0Ch` in
//  Initialize says.
//============================================================================
#include "text.h"
#include "tshashstr.inl"

template FONTHASHOBJ* CFontTable::Ptr(const char*);
template void         CFontTable::Initialize();
template int          CFontTable::CheckGrow(unsigned int);
template void         CFontTable::Rehash(unsigned int);
template FONTHASHOBJ* CFontTable::NewNode(unsigned int, void*, void*);
