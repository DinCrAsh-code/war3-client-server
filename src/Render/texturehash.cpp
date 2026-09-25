//============================================================================
//  TSHashTable<CTextureHash,HASHKEY_TEXTUREFILE>'s five loop-carrying
//  members, one translation unit because none of them can inline into
//  another and the shipped code has a real call at every edge between them:
//
//      0x6F4E10D0 Ptr, 0x6F4E2080 Initialize, 0x6F4E34D0 CheckGrow,
//      0x6F4E2200 Rehash, 0x6F4E38B0 NewNode.
//============================================================================
#include "texture.h"
#include "tshash.inl"

template CTextureHash* CTextureTable::Ptr(unsigned int,
                                          const HASHKEY_TEXTUREFILE&);
template void          CTextureTable::Initialize();
template int           CTextureTable::CheckGrow(unsigned int);
template void          CTextureTable::Rehash(unsigned int);
template CTextureHash* CTextureTable::NewNode(unsigned int, void*, void*);
