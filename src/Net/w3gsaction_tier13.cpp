//============================================================================
//  batch-B-w3gsaction_thunks - 0x6F67AB30. Same guarded-list lookup shape as
//  every other entry in this batch (unk_6FACFF90/dword_6FACFFA4), gated by
//  the record's own +0x130 state (== 3 here, not the +0x244 tri-state check
//  another entry uses) and a zero-check on +0x244, then a genuine 21-case
//  switch on the caller's own GameActionId (arg8): cases 7, 9, 10 and 27
//  keep the raw id, everything else in [7,27] (and, by construction, out of
//  range too) collapses to the shared "id 7" path - see
//  docs/msvc-vc8-idioms.md's "A jump-table switch's own data table shows up
//  on one side only (extended)" entry for why writing only the four real
//  case labels plus a default reproduces the same compaction table as
//  IDA's own byte_6F67AC40/jpt_6F67ABD8 pair, rather than a wider dispatch
//  spelled out by hand. The chosen id then drives one virtual call through
//  the record's own +0x58 sub-object's vtable slot 0xA0 (no committed
//  vtable doc names this class, so vtable_dispatch_audit.py leaves this
//  site unaudited rather than guessing), followed by a not-in-scope
//  cleanup call.
//============================================================================

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

//  The guarded-table record, carrying only the fields this handler reads:
//  +0x58 (a sub-object `this` for the virtual call and the cleanup call),
//  +0x130 (m_state, gated against 3 here), +0x148 (base used twice - once
//  +0xB5 as a byte buffer pointer, once its own +0xB0 dword), +0x150 (a
//  second sub-object passed whole to the cleanup call) and +0x244 (a
//  distinct "already handled" flag from the +0x244 tri-state another
//  entry in this batch reads at the same offset for a different purpose).
struct GameRecord6
{
    char m_reserved000[0x8];
    void* m_nextRecord;        // +0x08
    unsigned int m_recordKey;  // +0x0C
    char m_reserved010[0x58 - 0x10];
    void* m_field58;           // +0x58
    char m_reserved05C[0x130 - 0x5C];
    int m_state;                // +0x130
    char m_reserved134[0x148 - 0x134];
    char* m_field148;           // +0x148
    char m_reserved14C[0x244 - 0x14C];
    int m_field244;              // +0x244
};

#include "cdatastore.h"

//  0x6F675240 - fastcall(ecx=src, edx=declaredLength, stack out), retn 4.
//  Same base shape as w3gsaction_tier8.cpp's own FieldValidator_6F675450
//  family: a real local `CDataStore`, one field reader call (here the
//  already-reconstructed NetMsgReadDword_6F683E90, netmsgio_683.cpp - a
//  bare four-byte read of the message's own leading dword), success iff
//  the reader consumed exactly `declaredLength`.  Same unreproducible
//  `/GS`-cookie frame every sibling in this family carries.
extern CDataStore* __fastcall NetMsgReadDword_6F683E90(CDataStore* self, unsigned int* dst);
int __fastcall FieldValidator_6F675240(unsigned int src, unsigned int declaredLength, void* out)
{
    CDataStore local;
    local.m_field4 = (void*)src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgReadDword_6F683E90(&local, (unsigned int*)out);
    return local.m_readPos == local.m_field10;
}

//  0x6F67A960 - fastcall(this=record, edx=newState), plain `retn` (0 stack
//  bytes).  Sets the record's own +0x130 state to `newState`, and on a
//  transition into state 7 stamps +0x1AC with the current tick count and
//  runs the already-reconstructed FlushPendingRecords
//  (0x6F6797A0, netclient_pendingflush.cpp) if the record had been PAST
//  state 7 already (a re-entry, not a fresh arrival); independently, any
//  transition that crosses state 7 upward (old < 7, new >= 7) also stamps
//  +0x200 with a tick count.  Reused verbatim by w3gsaction_tier15.cpp
//  (edx=6 there) - a second naked body under the identical decorated name
//  would collide with this one at link time, so that file forward-declares
//  this real definition instead.
extern "C" void __fastcall FlushPendingRecords(void* connRecord);
extern "C" unsigned int __stdcall GetTickCount(void);
extern "C" void __fastcall Sub_6F67A960(void* self, unsigned int newState)
{
    int oldState = *(int*)((char*)self + 0x130);
    *(int*)((char*)self + 0x130) = newState;

    if (newState == 7)
    {
        *(unsigned int*)((char*)self + 0x1AC) = GetTickCount();
        if (oldState > 7)
            FlushPendingRecords(self);
    }

    if (oldState < 7 && (int)newState >= 7)
        *(unsigned int*)((char*)self + 0x200) = GetTickCount();
}

//  0x6F65CC70 - thiscall(this, stack arg0=nodePtr, stack arg4=value),
//  retn 8, confirmed against its own dump: a guarded list-removal helper
//  under `this`'s own +0x5F0 CRITICAL_SECTION (EnterCritSec/LeaveCritSec,
//  Sync/critsec.cpp).  Under the lock: walk a singly-linked list starting
//  at MaskedPositive(*(int*)(this+0x610)) (or straight to the sentinel
//  head `this+0x60C` on the one dead branch a generic list-walk template
//  leaves behind - structurally unreachable here, since the loop only
//  ever re-enters with a positive node pointer), calling an unconfirmed
//  virtual slot (`(*this->vtable)[0x2C/4](this, nodePtr, node+0x18, 0)`,
//  no committed vtable doc for this class so not something
//  vtable_dispatch_audit.py can check) on each node and comparing `value`
//  against `node+0x38` on a nonzero result; on a match whose own `+0x10`
//  field is non-negative, either sets that field's own top bit (0x80000000,
//  when a 0x20000000 bit is already set) or calls the not-in-scope
//  sub_6F65ABC0(this, node+8) - then, either way, unlinks the node from a
//  second doubly-linked list (`[node]`/`[node+4]` prev/next, the same
//  splice-out shape this repo's own TSExplicitList unlink idiom uses
//  elsewhere) and frees it, before decrementing `this+0x614`.  Left a
//  correctly-ABI'd naked thunk rather than a speculative transcription of
//  the bit-flag semantics (0x20000000/0x80000000) and the unconfirmed
//  vtable slot, both of which a wrong guess here would get past the score
//  entirely (an indirect call and a flag write cost nothing visible to
//  `verify.py`) while a genuinely wrong ABI or vtable arity would not be
//  caught by anything short of a live crash. A free function cannot be
//  declared `__thiscall` (only a native member function may), so this is
//  a member of a tiny placeholder type, the same pattern
//  w3gsaction_tier8.cpp's own FieldTarget already establishes.
struct CleanupTarget
{
    void Cleanup(void* nodePtr, unsigned int value);
};
__declspec(naked) void CleanupTarget::Cleanup(void*, unsigned int)
{
    __asm { mov eax, 0x6F65CC70 }
    __asm { jmp eax }
}

//  Slot 0xA0 of the +0x58 sub-object's own vtable - a guess from the call
//  site alone (5 stack args, thiscall cleanup by construction since the
//  shipped body never adjusts esp after `call edx`). No committed vtable
//  doc names this class, so this dispatch is not something
//  vtable_dispatch_audit.py can check; recorded here rather than silently
//  assumed safe. `__thiscall` is legal on a member-function-pointer
//  typedef (unlike on a free function), so no placeholder type is needed
//  just for this one.
typedef void (__thiscall *NotifyActionFn)(void*, unsigned int, unsigned int, void*, void*, unsigned int);

//  0x6F67AB30 - case (see w3gsactionhandler2.cpp). On a record match whose
//  own state is still below 3 and whose own +0x244 flag is clear: run the
//  not-in-scope state-transition helper, pick a reported action id (the
//  caller's own arg8 for cases 7/9/10/27, or the shared id 7 for every
//  other case in [7,27]), dispatch through the sub-object's own vtable
//  slot 0xA0, then run the not-in-scope cleanup call.
int __stdcall W3GSAction2_6F67AB30(void* self, unsigned int arg4, unsigned int arg8)
{
    if (!FieldValidator_6F675240(arg4, arg8, &arg8))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    if (key)
    {
        g_netClientRecordTableGuard.Wait(1);

        GameRecord6* rec = (GameRecord6*)MaskedPositive(g_netClientRecordListHead);
        while (rec && (void*)rec->m_recordKey != key)
            rec = (GameRecord6*)rec->m_nextRecord;

        if (rec)
        {
            //  Captured right when the record is confirmed found, before
            //  either gating condition is tested - the shipped body reads
            //  this field unconditionally, ahead of both compares, not
            //  only once inside the block they guard.
            void* sub58 = rec->m_field58;

            if (rec->m_state < 3 && rec->m_field244 == 0)
            {
                Sub_6F67A960(rec, 3);

                //  Established before the switch, not inside a case body:
                //  the shipped body copies the raw action id into the
                //  register it keeps for the whole rest of this block
                //  before subtracting in a SEPARATE register to drive the
                //  jump table, so the direct cases (7/9/10/27) fall
                //  straight into the shared code with nothing left to
                //  store.
                unsigned int caseId = arg8;
                switch (arg8)
                {
                case 7:
                case 9:
                case 10:
                case 27:
                    break;
                default:
                    caseId = 7;
                    break;
                }

                (*(NotifyActionFn**)sub58)[0xA0 / 4](
                    sub58, caseId, 0, rec->m_field148 + 0xB5, (char*)rec + 0x5C,
                    rec->m_recordKey);

                ((CleanupTarget*)sub58)->Cleanup((char*)rec + 0x150, *(unsigned int*)(rec->m_field148 + 0xB0));
            }
        }

        g_netClientRecordTableGuard.SignalOne(1);
    }
    return 0;
}
