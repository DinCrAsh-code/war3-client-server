//============================================================================
//  0x6F3F08C0 - CTechTree::Save, reached from CPlayerWar3::Save's own +0x2D0
//  field (playerwar3_save.cpp).  CTechTree itself is never modelled
//  concretely (playerwar3.h's own class comment, same reasoning as
//  CSelectionWar3) - every field this file touches is a raw offset read
//  straight off the dump.
//
//  Own translation unit: two registration-list writers
//  (CTechTreeWriteRegList7/2, 0x6F3EF450/0x6F3EF500 - "7"/"2" for how many
//  fields each writes per node) plus four raw WriteDwordAlt fields, all
//  real calls.
//============================================================================
#include "playerwar3.h"
#include "cdatastorescratch.h"

//  0x6F3EF450 - append a 16-bit placeholder count, then walk the node list
//  at `listDescriptor+0xC` writing seven raw fields per node (+0x14 via
//  WriteDword, +0x18/+0x1C/+0x20/+0x24/+0x28/+0x2C/+0x30 via WriteDwordAlt -
//  eight total, "7" undercounts by one but is kept as this session's own
//  working name), patching the count back at the end (WriteWordAt).
//
//  The "next" link is not a plain `node+4` pointer the way every other list
//  walk in this dump is - it is `*(int*)([listDescriptor+4] + node + 4)`, a
//  second base pointer out of the descriptor added to the node's own
//  address before the final `+4` dereference.  Reproduced exactly as the
//  shipped stream computes it (this session's scope gives no further
//  evidence for what that second base is), not smoothed into an ordinary
//  pointer walk.  The bound check (`movzx ecx, bx; cmp ecx, 0FFFF0h`) can
//  never trip - a zero-extended 16-bit count is always under 0x10000,
//  which is under 0xFFFF0 - reproduced anyway because the shipped stream
//  has it.  `retn 0`.
CDataStoreScratch* __fastcall CTechTreeWriteRegList7(CDataStoreScratch* store,
                                                      void* listDescriptor)
{
    unsigned int placeholderOffset = (unsigned int)store->m_field10;
    store->WriteWord(0);

    char* base = (char*)listDescriptor;
    int node = *(int*)(base + 0xC);
    unsigned int count = 0;

    if (node > 0)
    {
        for (;;)
        {
            if ((unsigned short)count >= 0xFFFF0)
                break;

            store->WriteDword(*(unsigned int*)((char*)node + 0x14));
            store->WriteDwordAlt(*(unsigned int*)((char*)node + 0x18));
            store->WriteDwordAlt(*(unsigned int*)((char*)node + 0x1C));
            store->WriteDwordAlt(*(unsigned int*)((char*)node + 0x20));
            store->WriteDwordAlt(*(unsigned int*)((char*)node + 0x24));
            store->WriteDwordAlt(*(unsigned int*)((char*)node + 0x28));
            store->WriteDwordAlt(*(unsigned int*)((char*)node + 0x2C));
            store->WriteDwordAlt(*(unsigned int*)((char*)node + 0x30));

            int relBase = *(int*)(base + 4);
            int next = *(int*)(relBase + node + 4);
            count++;
            if (next <= 0)
                break;
            node = next;
        }
    }

    store->WriteWordAt(placeholderOffset, (unsigned short)count);
    return store;
}

//  0x6F3EF500 - the same shape as CTechTreeWriteRegList7 above, two fields
//  per node (+0x14 via WriteDword, +0x18 via WriteDword) instead of seven.
//  `retn 0`.
CDataStoreScratch* __fastcall CTechTreeWriteRegList2(CDataStoreScratch* store,
                                                      void* listDescriptor)
{
    unsigned int placeholderOffset = (unsigned int)store->m_field10;
    store->WriteWord(0);

    char* base = (char*)listDescriptor;
    int node = *(int*)(base + 0xC);
    unsigned int count = 0;

    if (node > 0)
    {
        for (;;)
        {
            if ((unsigned short)count >= 0xFFFF0)
                break;

            store->WriteDword(*(unsigned int*)((char*)node + 0x14));
            store->WriteDword(*(unsigned int*)((char*)node + 0x18));

            int relBase = *(int*)(base + 4);
            int next = *(int*)(relBase + node + 4);
            count++;
            if (next <= 0)
                break;
            node = next;
        }
    }

    store->WriteWordAt(placeholderOffset, (unsigned short)count);
    return store;
}

//  0x6F3F08C0.
void CTechTree::Save(CDataStoreScratch* store)
{
    char* self = (char*)this;

    CTechTreeWriteRegList7(store, self + 4);
    CTechTreeWriteRegList2(store, self + 0x2C);

    store->WriteDwordAlt(*(unsigned int*)(self + 0x54));
    store->WriteDwordAlt(*(unsigned int*)(self + 0x58));
    store->WriteDwordAlt(*(unsigned int*)(self + 0x5C));
    store->WriteDwordAlt(*(unsigned int*)(self + 0x60));
}
