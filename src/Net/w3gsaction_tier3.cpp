//============================================================================
//  batch-B-w3gsaction_thunks - three more opcode handlers (0x6F67B660,
//  0x6F67B830, 0x6F67BB80) that all validate their own field(s) first, then
//  - only when self+0x40 (this dispatcher family's usual session key) is
//  non-null - look their own record up in the SAME guarded, singly-linked
//  table netclient_notifyevent_0x23.cpp's own PostEvent_0x23 already
//  reconstructs (unk_6FACFF90 the ConditionVariable guard, dword_6FACFFA4
//  the list head, MaskedPositive the same "treat <=0 as null" idiom that
//  file's own list walk uses) - a different record type than that file's
//  own NetClientNotifyRecord (this one is keyed the same way at +0xC/+8 but
//  reads/writes different fields past that), so declared locally here
//  rather than importing that file's own struct.
//
//  Each function's own validator (sub_6F675B30/sub_6F675D40) is one of
//  this batch's own addresses (batch-I-playertable-and-bnet).  Both are
//  the same family as w3gsaction_tier11.cpp's own FieldValidator_6F675030/
//  w3gsaction_tier15.cpp's own FieldValidator_6F6755B0 - a real CDataStore
//  local wrapping a single already-reconstructed field reader, success
//  iff the reader consumes exactly `declaredLength`, wrapped in the
//  unified SEH/`/GS`-cookie frame docs/msvc-vc8-idioms.md's own
//  "An `__except_handler4`-shaped frame this toolchain cannot reproduce"
//  section documents as unreproducible under this build's fixed `/GS-`.
//============================================================================
#include "cdatastore.h"
#include "cdatastorescratch.h"

//  0x6F683B80 - already reconstructed (netmsgio_683_empty.cpp): the
//  message struct has no serialised fields, so this validator's own
//  "field reader" is a no-op and success is unconditional (m_readPos
//  starts and stays at 0, matching m_field10's own starting value of the
//  caller-supplied declaredLength only when that length is itself 0 -
//  transcribed as-is, the same as the real body does, rather than
//  special-cased).
extern CDataStoreScratch* __fastcall NetMsgIoNoFields_6F683B80(CDataStoreScratch* self, const void* src);

//  0x6F684170 - already reconstructed (netmsgio_684.cpp): read the
//  leading dword of the message struct.
extern CDataStore* __fastcall NetMsgReadDword_6F684170(CDataStore* self, unsigned int* dst);

struct ConditionVariable
{
    void Wait(unsigned int value);
    void SignalOne(unsigned int value);
};

extern ConditionVariable g_netClientRecordTableGuard;   // unk_6FACFF90
extern int               g_netClientRecordListHead;     // dword_6FACFFA4

static void* MaskedPositive(int v)
{
    return (v > 0) ? (void*)v : 0;
}

//  0x6F675B30 - retn 4 (fastcall ecx=src/edx=declaredLength + one stack
//  pointer argument).
int __fastcall FieldValidator_6F675B30(void* src, unsigned int declaredLength, unsigned int* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgIoNoFields_6F683B80((CDataStoreScratch*)&local, out);
    return local.m_readPos == local.m_field10;
}

//  0x6F675D40 - same 3-argument shape as FieldValidator_6F675B30.
int __fastcall FieldValidator_6F675D40(void* src, unsigned int declaredLength, unsigned int* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgReadDword_6F684170(&local, out);
    return local.m_readPos == local.m_field10;
}

//  0x6F673710 - trivial: zero one field on the record.
void __fastcall ClearRecordField16C(void* record)
{
    *(int*)((char*)record + 0x16C) = 0;
}

//  A record in the same guarded table PostEvent_0x23 walks
//  (netclient_notifyevent_0x23.cpp), extended with the two extra fields
//  0x6F67B660's own handler reads/writes (+0x168, +0x24C) - offsets exact,
//  everything between left as reserved padding since nothing here reads it.
struct GameRecord
{
    char m_reserved000[0x8];
    void* m_nextRecord;        // +0x08
    unsigned int m_recordKey;  // +0x0C
    char m_reserved010[0x44 - 0x10];
    unsigned int m_field44;    // +0x44
    char m_reserved048[0x168 - 0x48];
    unsigned int m_field168;   // +0x168
    unsigned int m_field16C;   // +0x16C (ClearRecordField16C's own target)
    char m_reserved170[0x24C - 0x170];
    unsigned int m_field24C;   // +0x24C
};

//  0x6F67B660 - case (see w3gsactionhandler2.cpp). Validate one dword field
//  in place (`&arg8`, overwriting the caller's own declared-length slot with
//  the parsed value), then - if self+0x40 is set - look the record up and,
//  when +0x24C reads zero and +0x168 reads NONzero, run the one-shot
//  "trigger" (ClearRecordField16C, whose real shipped body per
//  sub_6F673710's own two instructions is: zero that record's own +0x16C)
//  and ALSO explicitly re-zero +0x168 itself (the shipped body's own extra
//  store - it does not rely on ClearRecordField16C touching that field, it
//  clears it a second time itself) before reporting status 2 instead of 0.
int __stdcall W3GSAction2_6F67B660(void* self, unsigned int arg4, unsigned int arg8)
{
    if (!FieldValidator_6F675B30((void*)arg4, arg8, &arg8))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    unsigned int status = 0;
    if (key)
    {
        g_netClientRecordTableGuard.Wait(1);

        GameRecord* rec = (GameRecord*)MaskedPositive(g_netClientRecordListHead);
        while (rec && (void*)rec->m_recordKey != key)
            rec = (GameRecord*)rec->m_nextRecord;

        if (rec && rec->m_field24C == 0 && rec->m_field168 != 0)
        {
            ClearRecordField16C(rec);
            rec->m_field168 = 0;
            status = 2;
        }

        g_netClientRecordTableGuard.SignalOne(1);
    }
    return (int)status;
}

//  0x6F67B830 - same validate-then-lookup shape as 0x6F67B660, but the
//  successful lookup just stores the parsed dword into the record's own
//  +0x44 field (no trigger call) and always returns 0.
int __stdcall W3GSAction2_6F67B830(void* self, unsigned int arg4, unsigned int arg8)
{
    if (!FieldValidator_6F675D40((void*)arg4, arg8, &arg8))
        return 1;

    void* self40 = *(void**)((char*)self + 0x40);
    if (self40)
    {
        g_netClientRecordTableGuard.Wait(1);

        GameRecord* rec = (GameRecord*)MaskedPositive(g_netClientRecordListHead);
        while (rec && (void*)rec->m_recordKey != self40)
            rec = (GameRecord*)rec->m_nextRecord;

        if (rec)
            rec->m_field44 = arg8;

        g_netClientRecordTableGuard.SignalOne(1);
    }
    return 0;
}

//  0x6F67BB80 - no field validator (the guard is a raw bit test on
//  self+0x80 instead); the successful lookup calls a not-in-scope raw copy
//  (sub_6F6EE820, `this=&record+0x10`, arg4, arg8) and, only if that copy
//  reports a non-zero record+0x14 afterwards, reports status 2.
//  0x6F6EE820 - `this` = record+0x10, two stack args. A separate tiny type
//  (rather than a member of CopyRecord below) purely so its `this` lands at
//  the right sub-object address without a same-offset field to take the
//  address of.
struct CopyTarget
{
    void RawCopyIn(unsigned int, unsigned int);
};
__declspec(naked) void CopyTarget::RawCopyIn(unsigned int, unsigned int)
{
    __asm { mov eax, 0x6F6EE820 }
    __asm { jmp eax }
}

struct CopyRecord
{
    char m_reserved00[0x10];
    char m_copyDest[0x4];   // +0x10, reinterpreted as CopyTarget for the call
    unsigned int m_field14; // +0x14, the copy's own "did anything change" flag
};

int __stdcall W3GSAction2_6F67BB80(void* self, unsigned int arg4, unsigned int arg8)
{
    if (!(*(unsigned int*)((char*)self + 0x80) & 0x100000))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    unsigned int status = 0;
    if (key)
    {
        g_netClientRecordTableGuard.Wait(0);

        CopyRecord* rec = (CopyRecord*)MaskedPositive(g_netClientRecordListHead);
        while (rec && *(void**)((char*)rec + 0xC) != key)
            rec = (CopyRecord*)(*(void**)((char*)rec + 8));

        if (rec)
        {
            ((CopyTarget*)&rec->m_copyDest)->RawCopyIn(arg4, arg8);
            if (rec->m_field14 != 0)
                status = 2;
        }

        g_netClientRecordTableGuard.SignalOne(0);
    }
    return (int)status;
}
