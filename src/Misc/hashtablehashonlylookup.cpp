//============================================================================
//  CUnit batch-16 (slot-103 closure).
//
//  0x6F323020 - a hash-table lookup on an as-yet-unidentified singleton
//  table (`dword_6FAB5698`), reached only from `sub_6F324920`.  The exact
//  same shape `tshash.inl`'s own `TSHashTable<T,K>::PtrHashOnly` documents
//  for 0x6F42CCF0: the bucket walk matches `m_hashval == hashval` alone,
//  and the second stack argument (the would-be key) is never read on
//  either side of this call, so it is provably dead here too.  Modelled
//  with raw offsets rather than a second template instantiation, since
//  this table's own element type is not established anywhere this call
//  tree reaches - only the fields this walk actually touches (the mask at
//  +0x24, the bucket array at +0x1C, each 0xC-byte bucket's own
//  link-offset word and terminator `m_prevlink`, and the found object's
//  own leading hashval dword) get names, per CLAUDE.md's own rule.
//============================================================================

#include "hashtablehashonly.h"

void* SRawHashTableView::LookupByHashOnly(unsigned int hashval,
                                          const void* /*key, dead*/)
{
    if (m_mask == 0xFFFFFFFF)
        return 0;

    //  Re-indexes m_buckets[hashval & m_mask] fresh at both use sites,
    //  matching TSHashTable::PtrHashOnly's own source shape (tshash.inl) -
    //  not cached in a local, which is what puts the shipped stream's own
    //  redundant re-load of the table/mask/bucket-base chain inside the
    //  loop rather than hoisting it once.
    int prevlink = *(int*)((char*)&m_buckets[hashval & m_mask] + 0x08);
    void* ptr = prevlink > 0 ? (void*)prevlink : 0;

    while (ptr != 0)
    {
        if (*(unsigned int*)ptr == hashval)
            return ptr;

        int link = *(int*)((char*)ptr +
            m_buckets[hashval & m_mask].m_linkoffset + 4);
        ptr = link > 0 ? (void*)link : 0;
    }

    return 0;
}
