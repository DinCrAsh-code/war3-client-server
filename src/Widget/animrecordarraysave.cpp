//============================================================================
//  0x6F4D5BB0 - SaveAnimRecordArray: the ring's own SAnimRecordArray storage
//  header (Widget/animtrackring.h) - the live count, then each record in
//  turn.  Part of CAnimTrackRing's Save family; see animtrackringsave.cpp
//  for the family's own header comment.
//============================================================================
#include "animtrackring.h"
#include "cdatastorescratch.h"

CDataStoreScratch* __fastcall SaveAnimTrackRecord(CDataStoreScratch* store,
                                                   SAnimTrackRecord* record);

CDataStoreScratch* __fastcall SaveAnimRecordArray(CDataStoreScratch* store,
                                                   SAnimRecordArray* records)
{
    unsigned int n = records->m_count;
    store->WriteDword(n);

    if (n > 0)
    {
        unsigned int offset = 0;
        do
        {
            SaveAnimTrackRecord(store,
                (SAnimTrackRecord*)((char*)records->m_data + offset));
            offset += 28;
        } while (--n != 0);
    }

    return store;
}
