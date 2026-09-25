//============================================================================
//  batch-H-w3gsaction-and-replaystream - 0x6F670170. Builds a real stack
//  CDataStore directly over the raw input (the same shape
//  W3GSAction_6F665580 already establishes) and reads a fixed 14-byte
//  record through the new NetMsgReadByteByteDwordDwordDword_6F6849A0
//  (netmsgio_684.cpp): targetId (byte0, searched for on a list), classId
//  (byte1, checked against self+0xB4), and lookupId (dword@2, an
//  ability/hero key for the already-reconstructed FindProgressRecordById -
//  w3gsaction_tier29.cpp).
//
//  Unlike every other ActionQueueTable-backed handler in this batch, the
//  record lookup only proceeds past a `record->field0xE4 < 3` gate when
//  `self` is exactly record->field0xD4's own (masked) head - a real,
//  dump-confirmed identity check, not a guess - and on a match it
//  dispatches the caller's own *raw input buffer* (bufferBase = arg4-4,
//  bufferLen = the low word of arg8+4) straight through
//  DispatchVtableSlot11, with no CDataStoreCache1460 reframing at all -
//  the one member of this whole tag family that forwards verbatim instead
//  of building its own frame.
//============================================================================
#include "cdatastore.h"

extern "C" void(__stdcall* g_pfnRecycleBuffer)(void** pField4, int* pField8, int* pField0xC);

//  0x6F6849A0 - already reconstructed for real (netmsgio_684.cpp).
extern CDataStore* __fastcall NetMsgReadByteByteDwordDwordDword_6F6849A0(CDataStore* self, unsigned char* dst);

//  0x6F66FF20 - already reconstructed for real (w3gsaction_tier29.cpp).
extern void* __fastcall FindProgressRecordById(void* record, unsigned int id);

//  0x6F668130 - already reconstructed for real (w3gsaction_tier10.cpp).
struct ListWalker
{
    void* Step(void* prev);
};

//  0x6F6DAE20 - already reconstructed for real elsewhere in this tree;
//  re-declared identically here for extern linkage.
extern void __fastcall DispatchVtableSlot11(void* self, int edxArg, unsigned int arg0);

//  Same ActionQueueTable/ConditionVariable infrastructure
//  w3gsaction_tier4.cpp and friends already establish.
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

static void* MaskedPositive(int v)
{
    return (v > 0) ? (void*)v : 0;
}

struct ByteByteDwordDwordDword
{
    unsigned char targetId;   // +0
    unsigned char classId;    // +1
    unsigned int  lookupId;   // +2
    unsigned int  reserved6;  // +6  - forwarded raw, not read back individually
    unsigned int  reserved10; // +10 - forwarded raw, not read back individually
};

//  0x6F670170 - case (see w3gsactionhandler2.cpp).
int __stdcall W3GSAction_6F670170(void* self, unsigned int arg4, unsigned int arg8)
{
    if (arg8 - 0xE > 0x5A2)
        return 1;

    CDataStore local;
    ByteByteDwordDwordDword fields;

    local.m_field4  = (void*)arg4;
    local.m_field8  = 0;
    local.m_field0xC = -1;
    local.m_field10 = arg8;
    local.m_readPos = 0;

    NetMsgReadByteByteDwordDwordDword_6F6849A0(&local, (unsigned char*)&fields);

    if ((unsigned int)local.m_readPos > (unsigned int)local.m_field10)
    {
        if (local.m_field0xC != -1)
            g_pfnRecycleBuffer(&local.m_field4, &local.m_field8, &local.m_field0xC);
        return 1;
    }

    if (local.m_field0xC != -1)
        g_pfnRecycleBuffer(&local.m_field4, &local.m_field8, &local.m_field0xC);

    if (fields.classId != *(unsigned char*)((char*)self + 0xB4))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    int wasCreated = 0;
    void* record = g_actionQueueTable.FindOrCreateRecord(key, 0, &wasCreated, 0, 1);

    int status = 0;
    if (record)
    {
        if (*(unsigned int*)((char*)self + 0x80) & 0x400000)
        {
            status = 2;
        }
        else if (*(int*)((char*)record + 0xE4) < 3 &&
                 self == MaskedPositive(*(int*)((char*)record + 0xD4)))
        {
            void* progressRecord = FindProgressRecordById(record, fields.lookupId);
            if (progressRecord)
            {
                int node = *(int*)((char*)record + 0xD4);
                node = (node > 0) ? node : 0;
                while (node > 0)
                {
                    if (*(unsigned char*)((char*)node + 0xB4) == fields.targetId)
                    {
                        void* target = *(void**)((char*)node + 0x3C);
                        void* bufferBase = (char*)arg4 - 4;
                        unsigned int bufferLen = (unsigned short)(arg8 + 4);
                        DispatchVtableSlot11(target, (int)bufferBase, bufferLen);
                        break;
                    }
                    node = *(int*)(*(char**)((char*)record + 0xCC) + node + 4);
                }
            }
            status = 2;
        }

        if (wasCreated != -1)
            g_actionQueueTable_Signal(wasCreated);
    }
    return status;
}
