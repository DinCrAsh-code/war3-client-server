//============================================================================
//  0x6F53E670 - "does the current game data carry a real value in the tag at
//  +0x2288, as opposed to one of its two 'nothing here' sentinels".
//
//  Eight callers across the 0x6F2Fxxxx/0x6F30xxxx/0x6F34xxxx modules use it
//  as a plain predicate.  What the tag *selects* is not established by any
//  dump here - only that its two negative answers are the FourCCs 'LOOP' and
//  'NONE' read most-significant byte first, and that anything else counts as
//  set.  A name for the setting itself would be a guess, so the function is
//  named for what it mechanically asks and the two sentinels are spelled out
//  as the constants the shipped code compares against.
//
//  The object is reached the same way every other thread-local game-data
//  read in this binary is (gamecontext.h): slot 0x0D of this thread's
//  pointer array, then +0x10, then +0x08.  The +0x10 hop is the same
//  "cache" indirection CGameDataContext::m_pCache names; the object behind
//  it is far larger than anything else here touches, so only the one field
//  this function reads is declared.
//============================================================================
#include "gamecontext.h"

namespace
{
    struct SGameDataRecord
    {
        char         m_reserved0000[0x2288];
        unsigned int m_tag;          // +0x2288
    };

    struct SGameDataCacheSlot
    {
        char              m_reserved00[8];
        SGameDataRecord*  m_pRecord;  // +0x08
    };

    struct SGameDataSlotRoot
    {
        char                 m_reserved00[0x10];
        SGameDataCacheSlot*  m_pCache;   // +0x10
    };
}

//  MSB first: 0x4C4F4F50 is 'LOOP', 0x4E4F4E45 is 'NONE'.
static const unsigned int kTagLoop = 0x4C4F4F50u;
static const unsigned int kTagNone = 0x4E4F4E45u;

int IsGameDataTagSet_6F53E670()
{
    SGameDataSlotRoot* root = (SGameDataSlotRoot*)GetThreadLocalSlot(0x0D);
    SGameDataRecord* record = root->m_pCache->m_pRecord;

    //  One shared `return 0` for all three negative answers, and the
    //  positive one as the fall-through.  Written the other way round - an
    //  early `return 0` per case - the compiler inverts the first branch and
    //  collapses the last comparison into a `setne`, which is three
    //  instructions the shipped stream does not have.
    if (record != 0)
    {
        unsigned int tag = record->m_tag;
        if (tag != kTagLoop && tag != kTagNone)
            return 1;
    }

    return 0;
}
