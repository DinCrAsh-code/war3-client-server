//============================================================================
//  CUnit batch-16 (slot-103 closure).
//
//  0x6F324920 - looks a hashval up in the unidentified `g_unk6FAB5698`
//  table (hashtablehashonlylookup.cpp) with a fixed, never-read debug-tag
//  key (`&g_unk6FAB4FC5`), and answers whether the record is missing or
//  stale: true if there is no record at all, or if the record's own +0x18
//  "expiry" tick is at or before the current tick count.
//============================================================================
#include "hashtablehashonly.h"

extern SRawHashTableView g_unk6FAB5698;
extern char g_unk6FAB4FC5;

extern "C" unsigned int __stdcall GetTickCount(void);

struct SExpiringCacheRecord
{
    char         m_reserved00[0x18];
    unsigned int m_expiryTick;  // +0x18
};

int __fastcall IsUnitTypeCacheRecordStale_6F324920(unsigned int hashval)
{
    SExpiringCacheRecord* record = (SExpiringCacheRecord*)
        g_unk6FAB5698.LookupByHashOnly(hashval, &g_unk6FAB4FC5);
    if (record != 0)
    {
        unsigned int now = GetTickCount();
        if (record->m_expiryTick > now)
            return 0;
    }
    return 1;
}
