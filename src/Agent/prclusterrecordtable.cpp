//============================================================================
//  0x6F4914B0/0x6F491520 - SaveRecordTable/LoadRecordTable (Agent/prcluster.h),
//  the whole-`SPrClusterRecordTable` Save/Load helpers CPrCluster::Save/Load
//  (prcluster_saveload.cpp) call once each.  Own translation unit from both
//  that one caller and from WriteClusterRecord/ReadClusterRecord themselves
//  (prclusterrecordio.cpp) - three distinct real `call`s in the shipped
//  dump, so none of the three files may see either of the other two's
//  definitions.
//
//  Both bodies cache `store` once and reuse it for every header write, the
//  same shape prclusterrecordio.cpp's own WriteClusterRecord/ReadClusterRecord
//  already establish - confirmed off the dump, not assumed.
//============================================================================
#include "prcluster.h"
#include "cdatastorescratch.h"
#include "cdatastore.h"

//  0x6F4914B0
CDataStoreScratch* __fastcall SaveRecordTable(CDataStoreScratch* store,
                                               SPrClusterRecordTable* table)
{
    store->WriteDword(table->m_block.m_blockSize);
    store->WriteDword(table->m_block.m_size);
    store->WriteDword(table->m_field14);
    store->WriteDword(table->m_field18);
    store->WriteDword(table->m_count);

    unsigned int i = 0;
    if (table->m_count > i)
    {
        unsigned int offset = 0;
        do
        {
            const SPrClusterRecord* rec =
                (const SPrClusterRecord*)((char*)table->m_block.m_data + offset);
            WriteClusterRecord(store, rec);
            i++;
            offset += sizeof(SPrClusterRecord);
        }
        while (i < table->m_count);
    }
    return store;
}

//  0x6F491520 - Save's exact mirror, except the block itself is freshly
//  (re)allocated via `CMemBlock::Init` (Pathfinding/cellbuffer.h) before the
//  element loop rather than merely walked - `header` is derived as
//  `m_blockSize - m_size` (`m_blockSize`'s own address is kept live across
//  every read in between, so this reads its just-streamed-in value, not a
//  stale one), matching the shipped body address for address.
CDataStore* __fastcall LoadRecordTable(CDataStore* store,
                                       SPrClusterRecordTable* table)
{
    store->ReadDword(&table->m_block.m_blockSize);
    store->ReadDword(&table->m_block.m_size);
    store->ReadDword(&table->m_field14);
    store->ReadDword(&table->m_field18);
    store->ReadDword(&table->m_count);

    unsigned int size = table->m_block.m_size;
    unsigned int header = table->m_block.m_blockSize - size;
    table->m_block.Init(size, header, 0, 0);

    if (table->m_count > 0)
    {
        unsigned int i = 0;
        unsigned int offset = 0;
        do
        {
            SPrClusterRecord* rec =
                (SPrClusterRecord*)((char*)table->m_block.m_data + offset);
            ReadClusterRecord(store, rec);
            i++;
            offset += sizeof(SPrClusterRecord);
        }
        while (i < table->m_count);
    }
    return store;
}
