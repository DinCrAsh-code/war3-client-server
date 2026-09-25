//============================================================================
//  batch-B-w3gsaction_thunks - one more handler (0x6F66FBE0) sharing
//  gameaction0x26_handler.cpp's own ActionQueueTable/ConditionVariable
//  infrastructure, same three-way state gate as
//  w3gsaction_tier4.cpp's own 0x6F66E240/0x6F66E430 (self+0x80 flag /
//  state-threshold / field0xD4-matches-key), this time state>=3 rather than
//  state<2 and no self+0x80 latch bit.
//
//  Its trigger, Sub_6F66FB70, is small enough (31 instructions, no SEH
//  frame) to be tractable on its own even though it is one call deeper than
//  this batch's own worklist - see its own header below.
//============================================================================
#include "storm.h"

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

//  0x6F6697D0/0x6F6698F0 - Sub_6F66FB70's own +0xCC sub-object callees.
//  Neither is one of this batch's own sixteen worklist addresses (they sit
//  one call deeper, off Sub_6F66FB70 rather than off a handler itself), and
//  0x6F6698F0 in particular opens its own real `__except_handler4`/
//  security-cookie prologue (92 instructions, confirmed from its own dump
//  tail) - the same confirmed SEH-frame ceiling
//  docs/targets/CUnit__vtable.md documents for CUnit's own thunked pair, so
//  it is not a reconstruction target.  0x6F6697D0 is a plain `retn` leaf
//  already thunked (and reused as-is) in w3gsaction_tier4.cpp.
//  Declared, not redefined - the one real body lives in
//  w3gsaction_tier4.cpp; a second `extern "C"` definition of the same
//  symbol here would be an LNK2005 duplicate at link time.
extern "C" void __fastcall Sub_6F6697D0(void*, unsigned int*);

extern "C" __declspec(naked) void __fastcall Sub_6F6698F0(void*, unsigned int*)
{
    __asm { mov eax, 0x6F6698F0 }
    __asm { jmp eax }
}

//  0x6F6C5550 - netproviderltcp_notify.cpp's own AcquireNotifyHandle,
//  reused here rather than re-thunked: same address, same fastcall(void)
//  shape confirmed from that file's own dump.
void* __fastcall AcquireNotifyHandle();

//  A minimal, forward-only file-info node: `TSList<Net::FileInfo,0>`'s own
//  element, embedded starting with its own TSLink (LINKOFFSET 0) - nothing
//  here ever dereferences one beyond Unlink()/SMemFree(), so it stays
//  incomplete past that.
namespace Net { struct FileInfo; }

//  edi's own class at Sub_6F66FB70's vtable dispatch (slot 0x4C/4 = 19) is
//  not confirmed - `record->m_field10` (this function's own edx) is some
//  object pointer the caller already had, and none of user_knowledge.json's
//  committed vtables put a plausible target at index 19 for it. The
//  argument *count* and order are not a guess - they come straight off the
//  call site's own three pushes - so this is written as an explicit slot
//  dispatch (vtable_dispatch_audit.py's own "no committed vtable" bucket:
//  reported as unknown, not validated) rather than invented as a named
//  method on a guessed class.
typedef void (__thiscall *Slot19Fn)(void* self, unsigned int a, unsigned int b, unsigned int c);

//  Same record shape as w3gsaction_tier4.cpp's own ActionQueueRecord2,
//  extended with the fields Sub_6F66FB70 itself reads/writes: +0x10 (the
//  trigger's own second, register argument), +0xC/+0xE0 (two dwords handed
//  to the vtable call), +0xF8 (where AcquireNotifyHandle's return value is
//  stashed) and +0x1F4 (a TSList<Net::FileInfo,0> drained via DestroyAll).
struct ActionQueueRecord3
{
    char m_reservedC[0xC];
    unsigned int m_fieldC;      // +0xC
    void* m_field10;            // +0x10, passed as Sub_6F66FB70's own edx
    char m_reservedCC[0xCC - 0x14];
    char m_subObjectCC[0x4];    // +0xCC, passed as `this` to Sub_6F6697D0/F0
    char m_reservedD4[0xD4 - 0xD0];
    unsigned int m_field0xD4;   // +0xD4
    char m_reservedE0[0xE0 - 0xD8];
    unsigned int m_field0xE0;   // +0xE0
    int  m_state;               // +0xE4
    char m_reservedF8[0xF8 - 0xE8];
    void* m_field0xF8;          // +0xF8
    char m_reserved1F4[0x1F4 - 0xFC];
    TSList<Net::FileInfo, 0> m_pendingFiles;   // +0x1F4
    char m_reserved200[0x400];
};

int __fastcall ParseDwordAction_6F666400(void* src, unsigned int declaredLength, unsigned int* out);

//  0x6F66FB70 - fastcall(this=record, edx=arg, an object whose vtable slot
//  19 gets dispatched below).  `retn` bare.
//
//  If the record hasn't already latched past state 1, run the +0xCC
//  sub-object's Sub_6F6697D0 once; then unconditionally bump state to 3,
//  register a notify handle and stash it at +0xF8, dispatch the caller's
//  own vtable slot 19 with (record->m_field0xE0, record->m_fieldC, 1), drain
//  the +0x1F4 pending-file list and run the same +0xCC sub-object's other
//  callee (Sub_6F6698F0) once more.
void __fastcall Sub_6F66FB70(ActionQueueRecord3* record, void* arg)
{
    //  One scratch dword, reused for both out-params below: their live
    //  ranges never overlap, and the shipped body reserves exactly one
    //  slot for them (a `push ecx` doing double duty as both the frame
    //  allocation and the discarded-`this`-alias spill, rather than two
    //  separate locals worth of `sub esp,8`).
    unsigned int scratch;

    if (record->m_state < 2)
        Sub_6F6697D0(record->m_subObjectCC, &scratch);

    record->m_state = 3;

    void* handle = AcquireNotifyHandle();
    unsigned int a = record->m_fieldC;
    unsigned int b = record->m_field0xE0;
    record->m_field0xF8 = handle;

    ((Slot19Fn)(*(void***)arg)[0x4C / 4])(arg, b, a, 1);

    record->m_pendingFiles.DestroyAll();

    Sub_6F6698F0(record->m_subObjectCC, &scratch);
}

//  0x6F66FBE0 - case (see w3gsactionhandler2.cpp). One-dword field
//  validator (already reconstructed, w3gsaction_parsers.cpp), then the
//  usual FindOrCreateRecord lookup: self+0x80 flag 0x400000 forces status 2
//  outright; else state>=3 forces status 1 (no trigger); else - only when
//  +0xD4, masked the same treat-negative-as-zero way as
//  w3gsaction_tier4.cpp's own two entries, equals the session key - runs
//  Sub_6F66FB70(record, record->m_field10) and reports status 2; anything
//  else reports status 1.
int __stdcall W3GSAction_6F66FBE0(void* self, unsigned int arg4, unsigned int arg8)
{
    if (*(int*)((char*)self + 0x40) == 0)
        return 1;

    unsigned int parsed;
    if (!ParseDwordAction_6F666400((void*)arg4, arg8, &parsed))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    int wasCreated = 0;
    ActionQueueRecord3* record = (ActionQueueRecord3*)
        g_actionQueueTable.FindOrCreateRecord(key, 1, &wasCreated, 0, 1);

    int status = 0;
    if (record)
    {
        if (*(unsigned int*)((char*)self + 0x80) & 0x400000)
        {
            status = 2;
        }
        else if (record->m_state >= 3)
        {
            status = 1;
        }
        else if ((void*)record->m_field0xD4 == key)
        {
            Sub_6F66FB70(record, record->m_field10);
            status = 2;
        }
        else
        {
            status = 1;
        }

        if (wasCreated != -1)
            ((ConditionVariable*)&g_actionQueueSignal)->SignalOne((unsigned int)(wasCreated != 0));
    }
    return status;
}

#include "tslist_destroy.inl"
template void TSList<Net::FileInfo, 0>::DestroyAll();
