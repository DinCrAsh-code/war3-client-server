//============================================================================
//  batch-B-w3gsaction_thunks - 0x6F680620. Same guarded-list lookup shape
//  as the rest of this batch; on a match with state<6, runs a three-step
//  reset (an optional ClearField0x16C_6F673710 gated by self's own +0x80
//  bit 0x100000 - the same flag-gated-extra-step shape
//  w3gsaction_tier7.cpp's own SetDeadlineFromNow call documents, a
//  different extra step here - then the not-in-scope Sub_6F67E960(record)
//  and Sub_6F67A960(record,6)) before the not-in-scope
//  Sub_6F65AE40(record->m_field58, 1) trigger.
//============================================================================
#include "cdatastore.h"
#include "cdatastorescratch.h"

CDataStoreScratch* __fastcall NetMsgIoNoFields_6F683B60(CDataStoreScratch* self, const void*);   // netmsgio_683_empty.cpp

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

struct GameRecord8
{
    char m_reserved000[0x8];
    void* m_nextRecord;        // +0x08
    unsigned int m_recordKey;  // +0x0C
    char m_reserved010[0x58 - 0x10];
    void* m_field58;           // +0x58
    char m_reserved05C[0x130 - 0x5C];
    int m_state;                // +0x130
};

//  0x6F6755B0 - fastcall(ecx=src, edx=declaredLength, stack out), retn 4.
//  Same shape as w3gsaction_parsers.cpp's own ParseNoFieldsAction family -
//  a local, non-owning CDataStore view over `src`, one field reader call,
//  success iff the reader consumed exactly `declaredLength` - except this
//  one builds a REAL `CDataStore` object (the vtable pointer is written
//  explicitly, not skipped the way the parsers.cpp manual-field pattern
//  does), so the local has a real destructor to run and the compiler
//  wraps it in the unified SEH/`/GS`-cookie frame
//  docs/msvc-vc8-idioms.md's "An `__except_handler4`-shaped frame this
//  toolchain cannot reproduce" section already documents as unreproducible
//  with this build's fixed `/GS-` - matches
//  ParseNoFieldsAction_6F6666C0's own 6/55 (0.154) shortfall for the same
//  reason. Field reader NetMsgIoNoFields_6F683B60 (netmsgio_683_empty.cpp).
int __fastcall FieldValidator_6F6755B0(void* src, unsigned int declaredLength, void* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgIoNoFields_6F683B60((CDataStoreScratch*)&local, out);
    return local.m_readPos == local.m_field10;
}

//  0x6F673710 - trivial: zero one field on the record. Reconstructed for
//  real (not a redirect) under two different C++ names elsewhere in
//  src/ already (src/Misc/trivialaccessors_06.cpp's own
//  ClearField0x16C_6F673710, w3gsaction_tier3.cpp's own
//  ClearRecordField16C) - this site now calls the real, already-hookable
//  trivialaccessors_06.cpp definition directly rather than keeping its
//  own naked redirect to the same address; whether that call gets
//  inlined here is the optimiser's call to make, not a source-level
//  decision this repo can force without a lying `__declspec(noinline)`.
//  Plain C++ linkage (no `extern "C"`), matching the real definition in
//  src/Misc/trivialaccessors_06.cpp - an `extern "C"` re-declaration here
//  mangled to a different symbol and left the .mix with an unresolved
//  external (link_check.py caught it).
void __fastcall ClearField0x16C_6F673710(void*);

//  0x6F67A960 - fastcall(this, edx), plain `retn`. Same callee
//  w3gsaction_tier13.cpp already declares with this exact 2-argument
//  signature (that file's own edx=3, this call site's own edx=6) -
//  reused by forward declaration rather than re-defined here, since a
//  second naked body under the identical decorated name
//  (`@Sub_6F67A960@8`) would collide with tier13's own at link time.
extern "C" void __fastcall Sub_6F67A960(void*, unsigned int);

//  0x6F67E8F0 - fastcall(this, edx) + one stack arg, retn 4. Out of scope
//  for this batch (not one of its own worklist addresses).
extern "C" __declspec(naked) void __fastcall RemoveFromWaitList_6F67E8F0(void*, void*, void*)
{
    __asm { mov eax, 0x6F67E8F0 }
    __asm { jmp eax }
}

//  0x6F67E960 - fastcall(this), plain `retn`. Walks a singly-linked list
//  rooted at self+0x288 (masked positive - a negative/zero value there
//  means "no list"), passing self and self's own +0x58 field to
//  RemoveFromWaitList_6F67E8F0 for every node; the loop's own re-test
//  after each step is a signed `> 0` against the raw `next` pointer
//  read from the node (not a `MaskedPositive` re-application), matching
//  the same jle/jg-vs-je/jne null-test-spelling class
//  w3gsaction_tier3.cpp's own header already documents.  The dump also
//  computes `&self->m_field284` into a register that nothing afterward
//  reads - dead in the shipped body itself, not just in this
//  reconstruction (an intrusive-list sentinel address the compiler
//  hoisted early and then never needed, most likely from an inlined
//  helper the loop no longer calls) - so it is not modelled here.
void __fastcall Sub_6F67E960(void* self)
{
    void* fixedArg = *(void**)((char*)self + 0x58);
    int rawHead = *(int*)((char*)self + 0x288);
    for (int node = (rawHead > 0) ? rawHead : 0; node > 0; )
    {
        int next = *(int*)(node + 4);
        RemoveFromWaitList_6F67E8F0(self, fixedArg, (void*)node);
        node = next;
    }
}

//  0x6F659890 - stdcall, retn 8. Same plain-SEH-plus-/GS-cookie frame
//  shape as the FieldValidator_ family above; not one of this batch's own
//  worklist addresses, so kept as a redirect rather than reconstructed.
extern "C" __declspec(naked) void __stdcall SetOneShotFlag_6F659890(unsigned int, unsigned int*)
{
    __asm { mov eax, 0x6F659890 }
    __asm { jmp eax }
}

//  0x6F65AE40 - thiscall(this, stack arg), retn 4 - the caller sets up
//  ecx (`mov ecx, edi`) even though the callee's own 7-instruction body
//  never reads it, the same "declared as a member, doesn't touch `this`"
//  shape as any other thiscall whose own leaf ignores the receiver. A
//  free function cannot be declared `__thiscall` (only a native member
//  function may), so this is a member of a tiny placeholder type, the
//  same pattern w3gsaction_tier8.cpp's own FieldTarget already
//  establishes. Body: reuses its own incoming stack slot as the
//  out-buffer for SetOneShotFlag_6F659890(0x1B, &value) - the shipped
//  body never allocates fresh stack space for it, it just writes the
//  same value straight back into the slot it just read.
struct TriggerTarget
{
    void Fire(unsigned int value);
};
void TriggerTarget::Fire(unsigned int value)
{
    unsigned int scratch = value;
    SetOneShotFlag_6F659890(0x1B, &scratch);
}

//  0x6F680620 - case (see w3gsactionhandler2.cpp). On a record match:
//  state<6 runs the reset sequence and reports status 2; state>=6
//  reports status 1; no match (or an absent key) reports status 0.
int __stdcall W3GSAction2_6F680620(void* self, unsigned int arg4, unsigned int arg8)
{
    if (!FieldValidator_6F6755B0((void*)arg4, arg8, &arg8))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    unsigned int status = 0;
    if (key)
    {
        g_netClientRecordTableGuard.Wait(1);

        GameRecord8* rec = (GameRecord8*)MaskedPositive(g_netClientRecordListHead);
        while (rec && (void*)rec->m_recordKey != key)
            rec = (GameRecord8*)rec->m_nextRecord;

        if (rec)
        {
            if (rec->m_state < 6)
            {
                //  Both the flag test and the +0x58 field load happen
                //  before the conditional branch in the shipped body (the
                //  load is unconditional, sitting between the `test` and
                //  the `jz` that reads its flags), matching this order.
                unsigned int flagBits = *(unsigned int*)((char*)self + 0x80);
                void* field58 = rec->m_field58;

                if (flagBits & 0x100000)
                    ClearField0x16C_6F673710(rec);

                Sub_6F67E960(rec);
                Sub_6F67A960(rec, 6);
                ((TriggerTarget*)field58)->Fire(1);

                status = 2;
            }
            else
            {
                status = 1;
            }
        }

        g_netClientRecordTableGuard.SignalOne(1);
    }
    return (int)status;
}
