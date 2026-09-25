//============================================================================
//  0x6F4D5330 - SaveAnimKeyArrayHeader: one record's own key-array int list
//  (Widget/animtrackring.h's SAnimKeyArrayHeader) - the count, then each key
//  in turn.  Part of CAnimTrackRing's Save family; see
//  animtrackringsave.cpp for the family's own header comment (why each
//  level is its own translation unit, why the fastcall shape, and what the
//  read side does not reconstruct).
//============================================================================
#include "animtrackring.h"
#include "cdatastorescratch.h"

CDataStoreScratch* __fastcall SaveAnimKeyArrayHeader(CDataStoreScratch* store,
                                                      SAnimKeyArrayHeader* keys)
{
    unsigned int n = keys->m_count;
    store->WriteDword(n);

    for (unsigned int i = 0; i < n; i++)
        store->WriteDwordAlt3(keys->m_keys[i]);

    return store;
}
