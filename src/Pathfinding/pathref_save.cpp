//============================================================================
//  0x6F47BCD0 - SPathRefArray::Save.  See widgetpathrefadjust.h.
//
//  Write the element count as a word, then every element through
//  WritePathRefEntry (pathref_write.cpp), then the array's own two CFloats
//  at +0x10/+0x14.  That tail is what named those two fields.
//
//  The loop is a byte-offset walk, not an index: the shipped code keeps a
//  running `esi` it adds 4 to and re-reads `m_pItems` through it each
//  iteration, and counts a separate zero-extended copy of the count down.
//
//  Own translation unit: three real calls per iteration plus two after.
//============================================================================
#include "widgetpathrefadjust.h"
#include "cdatastorescratch.h"

void __fastcall WritePathRefEntry(CDataStoreScratch* store, const void* entry);
CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store,
                                           const CFloat* value);

void SPathRefArray::Save(CDataStoreScratch* store)
{
    //  A word, not the whole dword - and the same narrowed value drives the
    //  loop, so a count above 65535 would write fewer entries than it
    //  declared.  Reproduced as the shipped code has it.
    unsigned short count = (unsigned short)m_count;
    store->WriteWord(count);

    //  `> 0` and not `!= 0`: the count is unsigned and the shipped guard is
    //  `jbe`, which `!= 0` lowers to `je`.
    if (count > 0)
    {
        unsigned int remaining = count;
        unsigned int offset = 0;
        do
        {
            WritePathRefEntry(store, *(void**)((char*)m_pItems + offset));
            offset += 4;
            remaining--;
        }
        while (remaining != 0);
    }

    //  Through one pointer to the pair, not two separate member addresses:
    //  the shipped code forms `lea esi,[edi+10h]` once and reaches the
    //  second CFloat as `[esi+4]`.
    const CFloat* pair = &m_x;
    WriteCFloat(store, pair);
    WriteCFloat(store, pair + 1);
}
