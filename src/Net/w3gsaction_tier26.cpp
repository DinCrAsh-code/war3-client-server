//============================================================================
//  batch-H-w3gsaction-and-replaystream - 0x6F66E4F0. The largest of this
//  batch's own handlers not already covered by batch-B/batch-J: a real
//  CDataStore field read (the same NameField16-wrapped
//  ReadPrefixedField_6F685230 W3GSAction_6F665580 already uses), a
//  CheckAndAdvanceInterval-gated interval computed from the *remaining*
//  bytes after that field, a class-id byte gate against self+0xB4, a
//  backward-walking name-byte validator (each byte either the sentinel
//  0xFF or in [1,0x10], and no two bytes equal), the same
//  ActionQueueTable::FindOrCreateRecord/ConditionVariable::SignalOne
//  infrastructure w3gsaction_tier4.cpp and w3gsaction_tier10.cpp already
//  establish, a throwaway CDataStoreScratch built and destroyed purely for
//  its own (not reconstructed - see cdatastorescratch.h) vtable-slot-0 side
//  effect, and a per-name-byte walk of the SAME record->field0xD4 linked
//  list w3gsaction_tier10.cpp's own W3GSAction_6F66E0E0 walks (via the same
//  ListWalker::Step, sub_6F668130), notifying at most one matching node per
//  byte through DispatchVtableSlot11 rather than tier10's own
//  notify-everyone-but-self.
//
//  Same unreproducible /GS-cookie + out-of-line-SEH-funclet frame every
//  other stack-CDataStore/CDataStoreScratch site in this tree already
//  carries.
//============================================================================
#include "cdatastore.h"
#include "cdatastorescratch.h"

extern "C" void(__stdcall* g_pfnRecycleBuffer)(void** pField4, int* pField8, int* pField0xC);

//  0x6F685230 - already reconstructed for real (netmsgio_684.cpp).
CDataStore* __fastcall ReadPrefixedField_6F685230(CDataStore* self, unsigned char* out);

//  0x6F663CC0 - already reconstructed for real (netclient_pendingflush.cpp).
extern int __fastcall CheckAndAdvanceInterval(int* state, unsigned int intervalMs,
                                               int allowEscalate, int* outLongSilence);

//  0x6F668130 - already reconstructed for real (w3gsaction_tier10.cpp).
struct ListWalker
{
    void* Step(void* prev);
};

//  0x6F6DAE20 - already reconstructed for real (netclient_notifyevent_0x23.cpp
//  and others); re-declared identically here for extern linkage.
extern void __fastcall DispatchVtableSlot11(void* self, int edxArg, unsigned int arg0);

//  Same ActionQueueTable/ConditionVariable infrastructure
//  w3gsaction_tier4.cpp and w3gsaction_tier10.cpp already establish.
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

static void g_actionQueueTable_Signal(int wasCreated)
{
    ((ConditionVariable*)&g_actionQueueSignal)->SignalOne((unsigned int)(wasCreated != 0));
}

//  0x6F664A40 - see cdatastorescratch.h's own declaration.
CDataStoreScratch* __fastcall CDataStoreScratch::WriteRecordHeader(unsigned char kind, unsigned short length, unsigned int unused)
{
    (void)unused;
    WriteByte(0xF7);
    WriteByte(kind);
    return WriteWord(length);
}

//  0x6F66E4F0 - case (see w3gsactionhandler2.cpp).
int __stdcall W3GSAction_6F66E4F0(void* self, unsigned int arg4, unsigned int arg8)
{
    void* key = *(void**)((char*)self + 0x40);
    if (key == 0)
        return 1;

    CDataStore local;
    unsigned char scratch[0x13];

    local.m_field4  = (void*)arg4;
    local.m_field8  = 0;
    local.m_field0xC = -1;
    local.m_field10 = arg8;
    local.m_readPos = 0;

    ReadPrefixedField_6F685230(&local, scratch);

    if ((unsigned int)local.m_readPos > (unsigned int)local.m_field10)
    {
        if (local.m_field0xC != -1)
            g_pfnRecycleBuffer(&local.m_field4, &local.m_field8, &local.m_field0xC);
        return 1;
    }

    unsigned int remaining = arg8 - local.m_readPos;

    if (local.m_field0xC != -1)
        g_pfnRecycleBuffer(&local.m_field4, &local.m_field8, &local.m_field0xC);

    if (remaining > 0x100)
        return 1;

    //  imul/shr 5 (÷32), floor-clamped to a minimum of 1000 - the same
    //  "remaining bytes -> an interval in milliseconds" scaling this
    //  session did not otherwise see a second instance of to name more
    //  concretely.
    unsigned int intervalMs = remaining * 1000u / 32u;
    if (intervalMs <= 1000u)
        intervalMs = 1000u;

    int dummyLongSilence = 2;
    if (!CheckAndAdvanceInterval((int*)((char*)self + 0x50), intervalMs, 0, &dummyLongSilence))
        return 2;

    if (scratch[0x11] != *(unsigned char*)((char*)self + 0xB4))
        return 1;

    unsigned int nameLen = scratch[0];
    unsigned int idx;
    if (nameLen != 0)
    {
        idx = nameLen;
        do
        {
            unsigned char ch = scratch[idx];
            idx -= 1;

            if (ch != 0xFF && (unsigned char)(ch - 1) > 0x0F)
                return 1;

            if (idx != 0)
            {
                unsigned int j = idx;
                do
                {
                    j -= 1;
                    if (scratch[1 + j] == ch)
                        return 1;
                } while (j != 0);
            }
        } while (idx != 0);
    }

    int wasCreated = 0;
    void* record = g_actionQueueTable.FindOrCreateRecord(key, 0, &wasCreated, 0, 1);

    int status = 0;
    if (record)
    {
        if (!(*(unsigned int*)((char*)self + 0x80) & 0x400000) &&
            *(int*)((char*)record + 0xE4) != 7)
        {
            void* bufferBase = (char*)arg4 - 4;
            unsigned int bufferLen = (unsigned short)(arg8 + 4);

            {
                CDataStoreScratch scratch2(bufferBase, (void*)bufferLen);
                scratch2.WriteRecordHeader(0x0F, (unsigned short)bufferLen, 0);
            }

            nameLen = scratch[0];
            if (nameLen != 0)
            {
                idx = nameLen;
                do
                {
                    unsigned char ch = scratch[idx];
                    idx -= 1;

                    int node = *(int*)((char*)record + 0xD4);
                    if (node > 0)
                    {
                        for (;;)
                        {
                            if (ch == 0xFF || *(unsigned char*)((char*)node + 0xB4) == ch)
                            {
                                if (node != 0)
                                {
                                    void* target = *(void**)((char*)node + 0x3C);
                                    DispatchVtableSlot11(target, (int)bufferBase, bufferLen);
                                }
                                break;
                            }
                            node = (int)((ListWalker*)((char*)record + 0xCC))->Step((void*)node);
                            if (node <= 0)
                                break;
                        }
                    }
                } while (idx != 0);
            }
        }

        status = 2;
        if (wasCreated != -1)
            g_actionQueueTable_Signal(wasCreated);
    }
    return status;
}
