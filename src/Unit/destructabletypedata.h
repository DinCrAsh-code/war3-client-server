//============================================================================
//  The per-type "destructable property" registry - a second, independent
//  TSHashTable<T,K> instantiation from CFootprintTypeData's own
//  (footprinttype.h), keyed by the same AGILE_TYPE_ID FourCC and hashed
//  with the same AGILE_TYPE_ID::Hash (0x6F4C8520, agileid.cpp/agiletype.h).
//
//  The node has no vtable of its own - TSHashObject<T,K>'s fields sit at
//  their un-shifted +0/+4/+0x14 (storm.h), matching what
//  destructabletypedata.cpp's own lookup reads (`cmp [eax], esi` against
//  the hash value at +0, `cmp [eax+14h], ...` against the key) - unlike
//  CFootprintTypeData, which carries a vtable and shifts everything by
//  four (footprinttype.h's own note on `HashLinkOffset()`).
//
//  Only the four fields CDestructable's own slots 66/69/94/95 read are
//  named; nothing else in this sweep touches the node.
//============================================================================
#ifndef DESTRUCTABLETYPEDATA_H
#define DESTRUCTABLETYPEDATA_H

#include "agiletype.h"

class CDestructableTypeData : public TSHashObject<CDestructableTypeData, AGILE_TYPE_ID>
{
public:
    int          m_reserved18;   // +0x18 - slot 95 (Method_0x17C)
    int          m_reserved1C;   // +0x1C - slot 69 (Method_0x114)
    char         m_reserved20[0x68 - 0x20];
    unsigned int m_reserved68;   // +0x68 - slot 94 (Method_0x178)
    char         m_reserved6C[0xA0 - 0x6C];
    float        m_reservedA0;   // +0xA0 - slot 66 (Method_0x108)
};

//  dword_6FAB4218 - funcmap.py's DATA table.
extern TSHashTable<CDestructableTypeData, AGILE_TYPE_ID> g_destructableTypeTable;

#endif
