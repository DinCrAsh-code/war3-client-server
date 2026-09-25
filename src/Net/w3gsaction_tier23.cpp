//============================================================================
//  batch-B-w3gsaction_thunks - 0x6F67CE30. The shipped body's own plain
//  /GS-cookie prologue/epilogue (a bare cookie XOR-check, not the full
//  __except_handler4 SEH frame w3gsaction_tier8.cpp/tier20.cpp document)
//  is still entirely unreproducible under this build's fixed /GS- -
//  documented, not skipped, the same as those. It also carries a
//  genuinely undeclared FOURTH argument arriving in edx, captured off
//  entry before the field validator's own edx use overwrites it - the
//  same hidden-register hazard w3gsaction_tier22.cpp's own header
//  documents for a hidden ecx, here in edx instead, and pointing at a
//  16-byte-displaced struct (four dwords) copied wholesale into the
//  record's own +0xA0..+0xAC.
//
//  On a match, this handler stamps a long run of the record's and self's
//  own fields from the field validator's parsed buffer, then genuinely
//  INSERTS `self` into the record's own +0x114 intrusive list - not the
//  array-indexed ListWalker idiom this batch's search-only entries use,
//  but a real splice, and it is exactly storm.h's own
//  `TSExplicitList<T>::LinkToHead` (the record's own +0x114/+0x118 pair
//  is that template's own m_linkoffset/m_terminator layout byte for
//  byte) - reused here rather than hand-derived, the same way this
//  codebase's other TSList-family call sites already do.
//============================================================================

#include "storm.h"
#include <string.h>          // memcpy

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

//  An incomplete type is enough: LinkToHead only ever forms and compares
//  pointers to it, never dereferences a field of it, so its own layout
//  need not be known here at all.
struct GameActionNode;

struct GameRecord16
{
    char m_reserved000[0x8];
    void* m_nextRecord;        // +0x08
    unsigned int m_recordKey;  // +0x0C
    char m_reserved010[0x58 - 0x10];
    void* m_field58;           // +0x58
    char m_reserved05C[0x10C - 0x5C];
    unsigned int m_field10C;    // +0x10C
    char m_reserved110[0x114 - 0x110];
    TSExplicitList<GameActionNode> m_actionList;  // +0x114 (m_linkoffset),
                                                    // +0x118 (m_terminator)
    char m_reserved120[0x12C - 0x120];
    unsigned int m_field12C;     // +0x12C
    char m_reserved130[0x140 - 0x130];
    unsigned int m_field140;      // +0x140 (own +0x130 state and +0x244
                                    // flag live BEFORE +0x140 - see below)
};
//  (m_state and m_field244 are read before m_field58, both below +0x114,
//  declared separately below to keep the struct's own field order
//  readable rather than interleaving unrelated regions.)

struct GameRecord16State
{
    char m_reserved000[0x130];
    int m_state;                // +0x130
    char m_reserved134[0x244 - 0x134];
    unsigned int m_field244;     // +0x244
};

long __fastcall InterlockedIncrementAt(volatile long* target);

//  0x6F6750E0 - fastcall(ecx, edx, stack ptr), retn 4. Same plain-SEH-
//  plus-/GS-cookie `CDataStore` byte-read shape as every other
//  FieldValidator_ in this batch, writing a run of packed fields into
//  its own out-buffer - out of scope, unreproducible under this build's
//  fixed /GS-.
extern "C" __declspec(naked) int __fastcall FieldValidator_6F6750E0(unsigned int, unsigned int, void*)
{
    __asm { mov eax, 0x6F6750E0 }
    __asm { jmp eax }
}

//  0x6F67C4A0 - thiscall(this) + 3 stack arguments, retn 0Ch. Allocates
//  and returns a new consumer node - out of scope for this batch.
extern "C" __declspec(naked) void* __fastcall AllocNode_6F67C4A0(void*, unsigned int, unsigned int, unsigned int)
{
    __asm { mov eax, 0x6F67C4A0 }
    __asm { jmp eax }
}

//  0x6F5451B0 - thiscall(this=dst) + 1 stack argument (src), retn 4 - NOT
//  fastcall despite this batch's own naked thunk having declared it that
//  way: the shipped body never reads `src` out of edx, only off
//  `[esp+4]`, so `edx` is genuinely unused and the second argument is a
//  real stack parameter. Copies a fixed 0x11-byte record header (four
//  dwords, then one length byte) verbatim, then that length byte's own
//  number of further raw bytes via memcpy - the same "fixed header, then
//  a length-prefixed variable tail" record shape this batch's other
//  record copies use.
//  netremainder2 session (batch-L closure pass): its only callee is
//  memcpy, so this is a real reconstruction now rather than out of scope.
//  Plain C++ linkage (not extern "C"): an `extern "C" __fastcall` symbol
//  has no readable-name comment in the MASM listing for extract_gen.py to
//  key off (docs/msvc-vc8-idioms.md, "extract_gen.py cannot resolve an
//  extern C __fastcall symbol's own PROC identity"), so the function
//  compiled and scored 0 as "not compiled into src/" until this switch -
//  same fix FindByKey_6F67EA80/Validate_6F679880 already needed.
//  `__thiscall` on a free function is an MSVC error (C3865: "can only be
//  used on native member functions"), so this is spelled as a member
//  function on a one-off local receiver type the way this file's other
//  thiscall-only leaves already do it.
struct RecordTailReceiver
{
    void CopyTail(void* src);
};
void RecordTailReceiver::CopyTail(void* src)
{
    unsigned char* d = (unsigned char*)this;
    unsigned char* s = (unsigned char*)src;

    *(unsigned int*)(d + 0x0) = *(unsigned int*)(s + 0x0);
    *(unsigned int*)(d + 0x4) = *(unsigned int*)(s + 0x4);
    *(unsigned int*)(d + 0x8) = *(unsigned int*)(s + 0x8);
    *(unsigned int*)(d + 0xC) = *(unsigned int*)(s + 0xC);
    unsigned char len = s[0x10];
    d[0x10] = len;

    memcpy(d + 0x11, s + 0x11, len);
}

//  0x6F67A960 - fastcall(this, edx), plain `retn`. Same callee
//  w3gsaction_tier13.cpp already declares with this exact 2-argument
//  signature; reused by forward declaration rather than re-defined here.
extern "C" void __fastcall Sub_6F67A960(void*, unsigned int);

//  0x6F65B7C0 - thiscall(this=record's own +0x58 sub-object) plus FIVE
//  stack arguments, retn 14h. Out of scope for this batch.
extern "C" __declspec(naked) void __fastcall Trigger_6F65B7C0(void*, unsigned int, unsigned int, void*, void*, unsigned int)
{
    __asm { mov eax, 0x6F65B7C0 }
    __asm { jmp eax }
}

//  0x6F673710 - trivial: zero one field on the record. Reconstructed for
//  real (not a redirect) under two other names elsewhere in src/ already
//  (trivialaccessors_06.cpp's ClearField0x16C_6F673710,
//  w3gsaction_tier3.cpp's ClearRecordField16C) - out of scope for this
//  TU's own trigger sequence, so redirected the same way this batch's
//  other out-of-scope callees are, matching the precedent
//  w3gsaction_tier15.cpp's own ClearField0x16C_6F680620 sets for a real
//  out-of-line `call` at this exact address.
extern "C" __declspec(naked) void __fastcall ClearField0x16C_6F67CE30(void*)
{
    __asm { mov eax, 0x6F673710 }
    __asm { jmp eax }
}

//  0x6F67CE30 - case (see w3gsactionhandler2.cpp). On a record match
//  whose own state < 3 and own +0x244 != 0, and whose own +0x10C bounds
//  the validator's parsed id byte (which must also be >= 1): stamps a
//  long run of record and self fields from the validator's own buffer
//  and from the hidden fourth argument's own +0x10 (four dwords), links
//  self into the record's own +0x114 intrusive list via
//  TSExplicitList::LinkToHead, bumps the record's own +0x12C counter and
//  self's own +0x10 refcount, allocates a consumer node
//  (AllocNode_6F67C4A0) and copies self's tail bytes into it
//  (CopyTail_6F5451B0), advances the record's own state to 4
//  (Sub_6F67A960), fires Trigger_6F65B7C0 and clears the record's own
//  +0x16C field, reporting status 2. Every other outcome reports 0/1 as
//  usual for this batch.
int __stdcall W3GSAction2_6F67CE30(void* self, unsigned int arg4, unsigned int arg8)
{
    void* hiddenArg4;
    __asm { mov hiddenArg4, edx }

    unsigned char buf[32];
    if (!FieldValidator_6F6750E0(arg4, arg8, buf))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    unsigned int status = 0;
    if (key)
    {
        g_netClientRecordTableGuard.Wait(1);

        GameRecord16* rec = (GameRecord16*)MaskedPositive(g_netClientRecordListHead);
        while (rec && (void*)rec->m_recordKey != key)
            rec = (GameRecord16*)rec->m_nextRecord;

        GameRecord16State* recState = (GameRecord16State*)rec;
        if (rec && recState->m_state < 3 && recState->m_field244 != 0)
        {
            unsigned char idByte = buf[8];
            if (idByte >= 1 && idByte <= rec->m_field10C)
            {
                void* field58 = rec->m_field58;

                rec->m_field140 = *(unsigned int*)(buf + 4);

                *(unsigned int*)((char*)self + 0x90) = *(unsigned int*)(buf + 9);
                *(unsigned int*)((char*)self + 0x94) = *(unsigned int*)(buf + 0xD);
                *(unsigned int*)((char*)self + 0x98) = *(unsigned int*)(buf + 0x11);
                *(unsigned int*)((char*)self + 0x9C) = *(unsigned int*)(buf + 0x15);

                unsigned int* extra = (unsigned int*)((char*)hiddenArg4 + 0x10);
                *(unsigned int*)((char*)self + 0xA0) = extra[0];
                *(unsigned int*)((char*)self + 0xA4) = extra[1];
                *(unsigned int*)((char*)self + 0xA8) = extra[2];
                *(unsigned int*)((char*)self + 0xAC) = extra[3];

                *(unsigned int*)((char*)self + 0xB0) = *(unsigned int*)buf;
                *(unsigned char*)((char*)self + 0xB4) = idByte;

                rec->m_actionList.LinkToHead((GameActionNode*)self);

                rec->m_field12C += 1;
                InterlockedIncrementAt((volatile long*)((char*)self + 0x10));

                void* newNode = AllocNode_6F67C4A0((char*)rec + 0x120, 2, 0, 0);
                *((unsigned char*)newNode + 8) = *(unsigned char*)((char*)self + 0xB4);
                ((RecordTailReceiver*)((char*)newNode + 9))->CopyTail((char*)self + 0xB5);

                Sub_6F67A960(rec, 4);

                Trigger_6F65B7C0(field58, 1, *(unsigned char*)((char*)self + 0xB4),
                                  (char*)self + 0xB5, (char*)rec + 0x5C, rec->m_recordKey);

                ClearField0x16C_6F67CE30(rec);

                status = 2;
            }
        }

        g_netClientRecordTableGuard.SignalOne(1);
    }
    return (int)status;
}
