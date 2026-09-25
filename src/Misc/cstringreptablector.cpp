//============================================================================
//  0x6F4C5290 - the string table's own constructor.
//
//  Written as a plain function rather than a C++ constructor: TSHashTable
//  declares two virtuals and defines neither (see storm.h's own note), so
//  constructing one in C++ would make MSVC emit a vtable for it and with it
//  an unresolved AllocNode.  The shipped body is a field-store sequence
//  either way.
//
//  The list is born with the poison link offset and then moved to 0x14 -
//  CStringRep::m_listlink - through SetLinkOffset rather than being written
//  directly, which is what puts the `cmp [edi], 14h` and the UnlinkAll
//  behind it in the stream.
//============================================================================
#include "cstringrep.h"

CStringRepTable* __fastcall CStringRepTableConstruct(CStringRepTable* table)
{
    *(const void**)table = g_vftableCStringRepTable;

    table->m_list.m_terminator.m_prevlink = 0;
    table->m_list.m_linkoffset = TSLIST_LINKOFFSET_POISON;
    table->m_list.Reset();

    table->m_buckets.m_alloc = 0;
    table->m_buckets.m_count = 0;
    table->m_buckets.m_data = 0;
    table->m_buckets.m_chunk = 0;
    table->m_probes = 0;

    table->m_list.SetLinkOffset(0x14);

    table->m_mask = 0xFFFFFFFFu;
    return table;
}
