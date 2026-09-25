//============================================================================
//  0x6F4D5500 - SaveAnimTrackRecord: one 28-byte SAnimTrackRecord
//  (Widget/animtrackring.h) - its own two leading dwords, then its nested
//  key-array header.  Part of CAnimTrackRing's Save family; see
//  animtrackringsave.cpp for the family's own header comment.
//============================================================================
#include "animtrackring.h"
#include "cdatastorescratch.h"

CDataStoreScratch* __fastcall SaveAnimKeyArrayHeader(CDataStoreScratch* store,
                                                      SAnimKeyArrayHeader* keys);

CDataStoreScratch* __fastcall SaveAnimTrackRecord(CDataStoreScratch* store,
                                                   SAnimTrackRecord* record)
{
    store->WriteDwordAlt(record->m_field0);
    store->WriteDwordAlt3(record->m_field4);
    SaveAnimKeyArrayHeader(store,
        (SAnimKeyArrayHeader*)((char*)record + 8));
    return store;
}
