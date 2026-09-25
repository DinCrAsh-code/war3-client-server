//============================================================================
//  0x6F4A7AB0 / 0x6F4A7B10 / 0x6F4A7BA0 - one shipped module:
//  `SInterfLisEntryArray`'s own `RemoveRange`, and the Save/Load leaves
//  `CPoPoInterfLis::Save`/`Load` (cpopointerflis_saveload.cpp) call for the
//  array's own header + every live element. See cpopointerflis.h's own
//  file header for the full shape.
//
//  `RemoveRange` mirrors Missile/bhpoprojectile_slots.cpp's own
//  `SBhPoTickRelaySlotArray::RemoveRange` exactly, just at a 0x0C-byte
//  stride instead of 4 - not reached from anything in this class's own
//  Slot4 with anything but `start=0`, but the shipped body computes the
//  general `[start, start+count)` shift regardless (`Slot4` is the only
//  caller in this dump, and always passes `count=m_count`, functionally a
//  full clear).
//============================================================================
#include "cpopointerflis.h"
#include "cdatastorescratch.h"
#include "cdatastore.h"

extern "C" void* __cdecl memmove(void* dst, const void* src, unsigned int size);

namespace NIpse {

int SInterfLisEntryArray::RemoveRange(unsigned int start, unsigned int count)
{
    if (start >= m_count)
        return 0;

    unsigned int clampedCount = count;
    if (start + clampedCount > m_count)
        clampedCount = m_count - start;

    unsigned int tail = m_count - start - clampedCount;
    if (tail != 0)
    {
        char* data = (char*)m_storage.m_data;
        memmove(data + start * 0x0C,
                data + (start + clampedCount) * 0x0C,
                tail * 0x0C);
    }

    m_count -= clampedCount;
    return 1;
}

CDataStoreScratch* __fastcall WriteInterfLisEntries(CDataStoreScratch* store,
                                                     SInterfLisEntryArray* array)
{
    store->WriteDword(array->m_storage.m_blockSize);
    store->WriteDword(array->m_storage.m_size);
    store->WriteDword(array->m_growBy);
    store->WriteDword(array->m_capacity);
    store->WriteDword(array->m_count);

    const SInterfLisEntry* entries =
        (const SInterfLisEntry*)array->m_storage.m_data;
    for (unsigned int i = 0; i < array->m_count; ++i)
    {
        store->WriteDword(entries[i].m_field0);
        store->WriteDword(entries[i].m_field4);
        store->WriteDwordAlt2(entries[i].m_field8);
    }

    return store;
}

CDataStore* __fastcall ReadInterfLisEntries(CDataStore* store,
                                            SInterfLisEntryArray* array)
{
    store->ReadDword(&array->m_storage.m_blockSize);
    store->ReadDword(&array->m_storage.m_size);
    store->ReadDword(&array->m_growBy);
    store->ReadDword(&array->m_capacity);
    store->ReadDword(&array->m_count);

    array->m_storage.Init(array->m_storage.m_size,
                           array->m_storage.m_blockSize - array->m_storage.m_size,
                           0, 0);

    SInterfLisEntry* entries =
        (SInterfLisEntry*)array->m_storage.m_data;
    for (unsigned int i = 0; i < array->m_count; ++i)
    {
        store->ReadDword(&entries[i].m_field0);
        store->ReadDword(&entries[i].m_field4);
        store->ReadDwordAlt3(&entries[i].m_field8);
    }

    return store;
}

}  // namespace NIpse
