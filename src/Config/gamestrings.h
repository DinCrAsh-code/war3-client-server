//============================================================================
//  The game-string table: the localised text `GetGameString` resolves keys
//  like "PAUSE_GAME_NOTIFY" and "KEY_PAUSE_GAME" out of.
//
//  There are two of them, side by side at 0x6FACD214 and 0x6FACD23C, and
//  they are the same shipped class: both are stamped with
//  `??_7?$TAllocatorHashTable@USTRINGHASHNODE@@VHASHKEY_STR@@$0BAA@@@6B@`
//  by their own constructors (0x6F862690 and 0x6F8626B0), i.e.
//  `TAllocatorHashTable<STRINGHASHNODE, HASHKEY_STR, 256>`.  So the class
//  name, the node name, the key name and the block size are all recovered
//  and none of them is invented.
//
//  Layout is TSHashTable's, which is what the offsets the lookup touches say
//  (buckets at +0x1C inside the growable array at +0x14, mask at +0x24, and
//  a vftable at +0x00 the constructors write).  It is modelled without
//  inheriting from TSHashTable and without a single `virtual`: this build
//  has no business emitting a vtable for a class it only ever reads through.
//============================================================================
#ifndef GAMESTRINGS_H
#define GAMESTRINGS_H

#include "storm.h"
//  The template, the two Storm string ordinals and the HASHKEY_STR key
//  policy all moved to Containers/allochashtable.h when the file cache's own
//  two instantiations of the same shipped template landed - see there for
//  where the class name comes from.
#include "allochashtable.h"

//----------------------------------------------------------------------------
//  One entry.  The hash and the intrusive link are TSHashObject's, the key
//  is at +0x14 where TSHashObject puts it, and the value - the only field
//  past the base - is at +0x1C.
//----------------------------------------------------------------------------
struct STRINGHASHNODE
{
    unsigned int             m_hashval;         // +0x00
    TSLink<STRINGHASHNODE>   m_hashlink;        // +0x04
    TSLink<STRINGHASHNODE>   m_listlink;        // +0x0C
    const char*              m_key;             // +0x14
    char                     m_reserved18[0x04];// +0x18
    const char*              m_value;           // +0x1C
};

typedef TAllocatorHashTable<STRINGHASHNODE, HASHKEY_STR, 256> CGameStringTable;

//: dword_6FACD214 - the base table, and dword_6FACD23C the override table
//: consulted first.  Both are the objects, not pointers to them: the shipped
//: code takes their address with `mov ecx, offset ...`.
extern CGameStringTable g_gameStrings;
extern CGameStringTable g_gameStringOverrides;

//  0x6F5C9650 - Config/gamestrings.cpp.
int __fastcall GetGameString(const char* key, char* buffer, int size);

#endif
