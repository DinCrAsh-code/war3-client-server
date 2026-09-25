//============================================================================
//  0x6F2B30B0 - ReadRecordTriples: WriteRecordTriples' counterpart
//  (item_saverecords.cpp), with three guards the writer has no need of.
//
//  The count the stream carries is checked against the array's *current*
//  count first - a bigger one is refused outright and the read position is
//  pushed one past the declared end, which poisons the rest of the load
//  rather than letting an oversized block walk off the array.  A current
//  count of zero is treated as four, this array's real size (position.h).
//
//  And the loop stops the moment the stream runs dry: the read position is
//  re-checked against the declared length after every entry, so a truncated
//  save loads a partial array instead of garbage.
//
//  Own translation unit: two real calls out of it.
//============================================================================
#include "cdatastore.h"
#include "handletriple.h"

struct SRecordTripleArray
{
    unsigned int  m_count;      // +0x00
    SHandleTriple m_items[1];   // +0x04
};

CDataStore* __fastcall ReadRecordTriples(CDataStore* store, void* array)
{
    SRecordTripleArray* arr = (SRecordTripleArray*)array;

    unsigned int limit = arr->m_count;
    if (limit == 0)
        limit = 4;

    //  Zeroed before the read and read into by address: the reader leaves
    //  its output alone when the stream is exhausted, so the zero is what
    //  makes an exhausted stream mean "no entries" rather than whatever was
    //  in the slot.
    unsigned int count = 0;
    store->ReadDword(&count);

    //  The declared length read once into a local and reused for both the
    //  guard and the poison value below - the shipped code keeps it in ecx
    //  across the comparison and then does `add ecx,1`.
    unsigned int length = (unsigned int)store->m_field10;
    if ((unsigned int)store->m_readPos > length)
        return store;

    if (count > limit)
    {
        store->m_readPos = (int)(length + 1);
        return store;
    }

    arr->m_count = count;
    //  `count > 0` and not `count != 0`: the shipped entry guard is `jbe`,
    //  the unsigned "<= 0" mask, which is what a `>` comparison on an
    //  unsigned gives and an inequality does not.
    if (count > 0)
    {
        SHandleTriple* p = arr->m_items;
        unsigned int i = 0;
        do
        {
            ReadHandleTriple(store, p);
            if ((unsigned int)store->m_readPos > (unsigned int)store->m_field10)
                return store;
            i++;
            p++;
        } while (i < count);
    }

    return store;
}
