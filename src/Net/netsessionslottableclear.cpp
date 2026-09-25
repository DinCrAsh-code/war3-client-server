//============================================================================
//  0x6F54B1A0 - CNetData::SLOTTABLE::ReleaseAll: empty the hash table.
//
//  The first 0x28 bytes of the slot table are Storm's TSHashTable header,
//  and this function is what says so - it zeroes the chain-length counter at
//  +0x10, drains the table-wide list at +0x04 through a real call to
//  TSExplicitList::UnlinkAll, and then walks the bucket array at
//  +0x18/+0x1C emptying each bucket in turn.
//
//  The `unlinkOnly` flag picks how: set, each bucket is drained by
//  TSLink::Unlink written out inline - which is UnlinkAll's own body, and
//  the shipped code inlines it here rather than calling it a second time;
//  clear, each tail is handed to the table's own vtable slot 0, which is
//  what actually frees a node.  Both loops re-read the bucket's tail every
//  time round rather than stepping a cursor, because both of them change it.
//
//  The slot-0 dispatch is written out by hand rather than as a `virtual`:
//  storm.h's TSHashTable declares its destructor in slot 0 and this table
//  plainly has something else there, and declaring a `virtual` this class
//  does not have would make this build stamp a vtable of its own (see
//  tools/abi_audit.py's VTABLE SYMBOL bucket).
//============================================================================
#include "netdata.h"
#include "storm.h"

//  Slot 0, called once per node with the node's own address.  Whatever it
//  is, it takes one stack argument and cleans it: the shipped call site
//  pushes exactly one word and does not clean it afterwards.
typedef void (__thiscall *SlotTableFreeNodeFn)(void* self, int node);

void CNetData::SLOTTABLE::ReleaseAll(int unlinkOnly)
{
    m_probes = 0;
    ((TSExplicitList<CNetData::CPlayer>*)&m_listLinkOffset)->UnlinkAll();

    for (unsigned int i = 0; i < m_bucketCount; i++)
    {
        for (;;)
        {
            TSExplicitList<CNetData::CPlayer>* bucket =
                (TSExplicitList<CNetData::CPlayer>*)((char*)m_buckets + i * 12);

            int tail = bucket->m_terminator.m_prevlink;
            if (tail <= 0)
                break;

            if (unlinkOnly)
            {
                ((TSLink<CNetData::CPlayer>*)(bucket->m_linkoffset + tail))
                    ->Unlink();
            }
            else
            {
                ((SlotTableFreeNodeFn)(*(void***)this)[0])(this, tail);
            }
        }
    }
}
