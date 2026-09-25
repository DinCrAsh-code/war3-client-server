//============================================================================
//  TSHashTable<CSolidTextureHash,HASHKEY_NONE>'s five loop-carrying members:
//
//      0x6F4E1150 Ptr, 0x6F4E2140 Initialize, 0x6F4E3570 CheckGrow,
//      0x6F4E23D0 Rehash, 0x6F4E3950 NewNode.
//
//  The lookup is one instruction pair shorter than the texture table's twin
//  and that is the whole difference the key policy makes: HASHKEY_NONE's
//  compare is a constant, so `ptr->m_hashval == hashval && ptr->m_key == key`
//  folds to the hash test alone and the key argument is never read - which is
//  exactly what 0x6F4E1150 does with the `offset unk_6FAB8020` its one caller
//  still pushes.
//============================================================================
#include "texture.h"
#include "tshash.inl"

template CSolidTextureHash* CSolidTextureTable::Ptr(unsigned int,
                                                    const HASHKEY_NONE&);
template void               CSolidTextureTable::Initialize();
template int                CSolidTextureTable::CheckGrow(unsigned int);
template void               CSolidTextureTable::Rehash(unsigned int);
template CSolidTextureHash* CSolidTextureTable::NewNode(unsigned int, void*,
                                                        void*);
