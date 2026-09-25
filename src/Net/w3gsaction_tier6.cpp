//============================================================================
//  batch-B-w3gsaction_thunks - one more handler (0x6F66E7A0) sharing
//  gameaction0x26_handler.cpp's own ActionQueueTable/ConditionVariable
//  infrastructure, this time with a record-owned CRITICAL_SECTION
//  (+0x188, the same lock gameaction0x26_handler.cpp's own
//  ActionQueueRecord::m_lock already names) guarding a running counter
//  comparison rather than a field0xD4-matches-key test.
//============================================================================
#include <windows.h>

struct ConditionVariable
{
    void SignalOne(unsigned int value);   // sub_6F6D8950
};
extern "C" unsigned int g_actionQueueSignal;   // unk_6FACFF20

struct ActionQueueTable
{
    void* FindOrCreateRecord(void* key, unsigned int arg2, int* wasCreated,
                              unsigned int arg4, unsigned int createIfMissing);
};
extern ActionQueueTable g_actionQueueTable;   // stru_6FACFF00

void __fastcall EnterCritSec(CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs);

//  Same record shape as gameaction0x26_handler.cpp's own ActionQueueRecord
//  (m_state@0xE4, m_lock@0x188), plus the two extra fields this handler
//  reads: +0x1C8 (a nullable pointer field, same "registered consumer"
//  shape that file's own m_consumerTarget documents at the same offset -
//  reused here under its own name since this TU does not otherwise share
//  that header) and +0x1CC (a count this handler compares self's own
//  running total against).
struct ActionQueueRecord4
{
    char m_reserved00[0xE4];
    int  m_state;                // +0xE4
    char m_reservedE8[0x188 - 0xE4 - 4];
    char m_lock[0x1C8 - 0x188];  // +0x188, a CRITICAL_SECTION
    void* m_consumerTarget;      // +0x1C8
    unsigned int m_field1CC;     // +0x1CC
};

int __fastcall ParseNoFieldsAction_6F6666C0(void* src, unsigned int declaredLength, void* out);

//  0x6F663FC0 - thiscall(this=record->m_consumerTarget), one stack byte
//  argument (retn 4).  `this` is a small fixed 16-slot ring buffer: a
//  capacity dword at +0, sixteen 12-byte entries starting at +4 (only
//  +4 - a player-id byte - and +5 - a saturating 0..16 hit-count byte -
//  are ever touched; the rest of each entry is an un-investigated
//  further subsystem), a head index at +0xC4, and a live-entry count at
//  +0xCC.  Linearly scans the `count` live entries starting at `head`
//  (not wrapped - the buffer is walked straight past `head`, so the
//  16-slot capacity is what actually bounds it) for one whose own +4
//  byte equals `playerId`; a hit bumps its own +5 count (saturating at
//  16), a miss appends a fresh entry (count 1) at `head+count` if that
//  index is still under the capacity dword, silently dropping the
//  notification otherwise.
struct ConsumerTarget
{
    void NotifyPlayerId(unsigned int);
};

void ConsumerTarget::NotifyPlayerId(unsigned int playerIdArg)
{
    unsigned char playerId = (unsigned char)playerIdArg;
    char* self = (char*)this;
    unsigned int size = *(unsigned int*)(self + 0xCC);
    unsigned int head = *(unsigned int*)(self + 0xC4);

    if (size > 0)
    {
        for (unsigned int i = 0; i < size; ++i)
        {
            unsigned int idx = head + i;
            char* entry = self + idx * 12;
            if (*(unsigned char*)(entry + 4) == playerId)
            {
                if (*(unsigned char*)(entry + 5) < 0x10)
                    *(unsigned char*)(entry + 5) += 1;
                return;
            }
        }
    }

    unsigned int freeIdx = *(unsigned int*)(self + 0xC4) + size;
    if (freeIdx >= *(unsigned int*)self)
        return;

    *(unsigned char*)(self + freeIdx * 12 + 4) = playerId;
    unsigned int freeIdx2 = *(unsigned int*)(self + 0xCC) + *(unsigned int*)(self + 0xC4);
    *(unsigned char*)(self + freeIdx2 * 12 + 5) = 1;
    *(unsigned int*)(self + 0xCC) += 1;
}

//  0x6F66E7A0 - case (see w3gsactionhandler2.cpp). No-fields validator
//  (already reconstructed), then FindOrCreateRecord; self+0x80 flag
//  0x400000 or record state==7 both force status 2 outright. Otherwise,
//  under the record's own lock: bump self's own running counter
//  (self+0x84) and, only while it is still within the record's own +0x1CC
//  bound AND the record has a registered consumer (+0x1C8), notify it with
//  self's own player id (self+0xB4) and report status 2; past the bound,
//  or with no consumer, the lock is still taken and released but nothing
//  else happens and status stays 0.
int __stdcall W3GSAction_6F66E7A0(void* self, unsigned int arg4, unsigned int arg8)
{
    if (*(int*)((char*)self + 0x40) == 0)
        return 1;

    void* unusedOut;
    if (!ParseNoFieldsAction_6F6666C0((void*)arg4, arg8, &unusedOut))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    int wasCreated = 0;
    ActionQueueRecord4* record = (ActionQueueRecord4*)
        g_actionQueueTable.FindOrCreateRecord(key, 0, &wasCreated, 0, 1);

    int status = 0;
    if (record)
    {
        if ((*(unsigned int*)((char*)self + 0x80) & 0x400000) || record->m_state == 7)
        {
            status = 2;
        }
        else
        {
            EnterCritSec((CRITICAL_SECTION*)record->m_lock);

            unsigned int running = ++*(unsigned int*)((char*)self + 0x84);
            if (running <= record->m_field1CC && record->m_consumerTarget)
            {
                unsigned char playerId = *((unsigned char*)self + 0xB4);
                ((ConsumerTarget*)record->m_consumerTarget)->NotifyPlayerId(playerId);
                status = 2;
            }

            LeaveCritSec((CRITICAL_SECTION*)record->m_lock);
        }

        if (wasCreated != -1)
            ((ConditionVariable*)&g_actionQueueSignal)->SignalOne((unsigned int)(wasCreated != 0));
    }
    return status;
}
