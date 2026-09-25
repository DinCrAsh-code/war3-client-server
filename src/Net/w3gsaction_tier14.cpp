//============================================================================
//  batch-B-w3gsaction_thunks - 0x6F680870. Same guarded-list lookup shape
//  as the rest of this batch (unk_6FACFF90/dword_6FACFFA4), keyed by
//  self+0x40 this time against the record's own +0x0C field directly
//  (not a `->m_recordKey` compared through a cast, since the shipped
//  body's own loop variable is the record pointer itself, reused in
//  place, the same as w3gsaction_tier9.cpp's ListWalker idiom); on a
//  match, the not-in-scope removal helper Sub_6F67FE10's own return
//  value picks status 2 vs 0.
//============================================================================

#include "cdatastore.h"
#include "cdatastorescratch.h"

//  0x6F683BA0 - already reconstructed (netmsgio_683_empty.cpp): the
//  message struct has no serialised fields.
extern CDataStoreScratch* __fastcall NetMsgIoNoFields_6F683BA0(CDataStoreScratch* self, const void* src);

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

struct GameRecord7
{
    char m_reserved000[0x8];
    void* m_nextRecord;        // +0x08
    unsigned int m_recordKey;  // +0x0C
};

//  0x6F675C90 - fastcall(ecx=src, edx=declaredLength, stack out), retn 4.
//  Same family as w3gsaction_tier3.cpp's own FieldValidator_6F675B30/
//  FieldValidator_6F675D40 - a real CDataStore local wrapping a single
//  already-reconstructed field reader (NetMsgIoNoFields_6F683BA0, the
//  message struct has no serialised fields), success iff m_readPos ==
//  m_field10, wrapped in the unified SEH/`/GS`-cookie frame
//  docs/msvc-vc8-idioms.md documents as unreproducible under this build's
//  fixed `/GS-`.
int __fastcall FieldValidator_6F675C90(void* src, unsigned int declaredLength, void* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgIoNoFields_6F683BA0((CDataStoreScratch*)&local, out);
    return local.m_readPos == local.m_field10;
}

//  0x6F67FE10 - thiscall(this), plain `retn` (0 stack bytes). A
//  guarded-list removal helper: 211 instructions reaching an indirect
//  vtable call through the record's own vtable+0x24 and vtable+8 slots
//  (two foreign, un-investigated overrides), a `memmove`-based buffer
//  compaction, and two further un-investigated cleanup leaves
//  (sub_6F6EE790/sub_6F6EE540) - genuinely below the confidence bar this
//  session can clear for a class whose own field layout this call tree
//  never establishes past the two offsets w3gsaction_tier3.cpp's own
//  CopyTarget already reaches (+0x10/+0x14). A wrong argument count or
//  vtable slot guess here is exactly the live-crash class
//  vtable_dispatch_audit.py/thunk_abi_audit.py exist to catch, so this
//  stays a thunk to the real, unhooked body rather than a guess - the
//  same judgement CLAUDE.md's own vtable-slot guidance makes explicit
//  ("guessing... would be worse than not looking").
struct RemovalTarget
{
    int Remove();
};
__declspec(naked) int RemovalTarget::Remove()
{
    __asm { mov eax, 0x6F67FE10 }
    __asm { jmp eax }
}

//  0x6F680870 - case (see w3gsactionhandler2.cpp). On a record match:
//  status 2 if the not-in-scope removal returns nonzero, status 0
//  otherwise (including no match, or an empty/absent key) - the shipped
//  body's own `edi` return value is zeroed once, up front, and only ever
//  set to 2 on the one path that finds and removes a live record.
int __stdcall W3GSAction2_6F680870(void* self, unsigned int arg4, unsigned int arg8)
{
    if (!FieldValidator_6F675C90((void*)arg4, arg8, &arg8))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    unsigned int status = 0;
    if (key)
    {
        g_netClientRecordTableGuard.Wait(1);

        GameRecord7* rec = (GameRecord7*)MaskedPositive(g_netClientRecordListHead);
        while (rec && (void*)rec->m_recordKey != key)
            rec = (GameRecord7*)rec->m_nextRecord;

        if (rec && ((RemovalTarget*)rec)->Remove())
            status = 2;

        g_netClientRecordTableGuard.SignalOne(1);
    }
    return (int)status;
}
