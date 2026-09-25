//============================================================================
//  0x6F03F940 - IsNetDataFlagSet.  The same per-thread net-data slot
//  (GetThreadLocalSlot(0x0D), gamecontext.h) NetDataRecords() reaches
//  (Net/netdata.h), but one level deeper: through the cache slot's own
//  +0x00 pointer (not +0x08's m_pRecords) to a small record whose +0x04
//  dword this masks against the caller's own flag.  Reached only from
//  CAbility::EvaluateDepCheckAlias (ability_depcheckalias.cpp).  Own
//  translation unit: 0x6F03F9xx is not the ability module (0x6F052Cxx).
//============================================================================
#include "gamecontext.h"

struct SThreadContextRoot
{
    char  m_reserved00[0x10];
    void* m_pCache;   // +0x10
};

struct SThreadContextFlagsRecord
{
    char         m_reserved0[4];
    unsigned int m_flags;   // +0x04
};

int __fastcall IsNetDataFlagSet(unsigned int mask)
{
    SThreadContextRoot* root = (SThreadContextRoot*)GetThreadLocalSlot(0x0D);
    void* cache = root->m_pCache;
    SThreadContextFlagsRecord* record = *(SThreadContextFlagsRecord**)cache;

    if (record != 0 && (record->m_flags & mask) != 0)
        return 1;
    return 0;
}
