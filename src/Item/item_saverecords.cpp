//============================================================================
//  0x6F2B3130 - WriteRecordTriples: write a counted array of three-dword
//  records into a CDataStoreScratch.
//
//  The count first, then three dwords per entry.  This is what finally
//  settles Position's own +0x98/+0x9C block (position.h): +0x98 is a count,
//  initialised to 4, and +0x9C..+0xC8 is exactly four {a,b,c} triples,
//  which is why the constructor writes (-1,-1,0) four times running.
//
//  A __fastcall free function, not a member of either side: the store
//  arrives in ecx and the array in edx and it cleans nothing, which neither
//  a store method nor an array method would produce.
//
//  Own translation unit: the per-element WriteDword call is real.
//============================================================================
#include "cdatastorescratch.h"

struct SRecordTriple
{
    unsigned int m_a;
    unsigned int m_b;
    unsigned int m_c;
};

struct SRecordTripleArray
{
    unsigned int  m_count;      // +0x00
    SRecordTriple m_items[1];   // +0x04
};

CDataStoreScratch* __fastcall WriteRecordTriples(CDataStoreScratch* store,
                                                 const void* array)
{
    const SRecordTripleArray* arr = (const SRecordTripleArray*)array;

    //  The count read once into a local and both written and used as the
    //  loop bound from there - the shipped code keeps it in ebx across the
    //  first call rather than re-reading +0x00, which is what a local gives.
    unsigned int count = arr->m_count;
    store->WriteDword(count);

    //  The third component goes through a *different* four-byte writer
    //  (0x6F4C2310) from the first two (0x6F4C2360) - see
    //  cdatastorescratch.h on why four identical writers exist.
    const SRecordTriple* p = arr->m_items;
    while (count > 0)
    {
        store->WriteDword(p->m_a);
        store->WriteDword(p->m_b);
        store->WriteDwordAlt2(p->m_c);
        p++;
        count--;
    }

    return store;
}
