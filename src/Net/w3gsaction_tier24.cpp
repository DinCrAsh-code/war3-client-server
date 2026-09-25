//============================================================================
//  batch-B-w3gsaction_thunks - 0x6F67D040. Same shape as
//  w3gsaction_tier23.cpp's own 0x6F67CE30 sibling right through the
//  TSExplicitList::LinkToHead insert and the consumer-node allocation -
//  same guarded-list lookup (state<3/+0x244!=0/+0x10C-bounded id byte
//  gate), same hidden-fourth-argument-in-edx hazard (captured the same
//  way, DIFFERS for the same reason), same field validator shape (this
//  one's own 0x98-byte out-buffer, pre-zeroed for the same two leading
//  bytes tier8.cpp's own FieldTarget header documents the validator only
//  conditionally filling). Two real differences from that sibling: no
//  self->0xB0 store here at all (this handler's own buffer never reads a
//  field for it), and the final trigger is a genuine VTABLE dispatch
//  through the record's own +0x58 sub-object's own slot 0xA0 (the same
//  slot w3gsaction_tier13.cpp's own 0x6F67AB30 already dispatches, no
//  committed vtable doc names the class either there or here, so
//  vtable_dispatch_audit.py leaves this site unaudited) rather than a
//  fixed callee, followed by a second call reusing
//  w3gsaction_tier8.cpp's own FieldTarget::ResetField on the validator's
//  own out-buffer.
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

#include "storm.h"

struct GameActionNode2;

struct GameRecord17
{
    char m_reserved000[0x8];
    void* m_nextRecord;        // +0x08
    unsigned int m_recordKey;  // +0x0C
    char m_reserved010[0x58 - 0x10];
    void* m_field58;           // +0x58
    char m_reserved05C[0x10C - 0x5C];
    unsigned int m_field10C;    // +0x10C
    char m_reserved110[0x114 - 0x110];
    TSExplicitList<GameActionNode2> m_actionList;  // +0x114/+0x118
    char m_reserved120[0x12C - 0x120];
    unsigned int m_field12C;     // +0x12C
    char m_reserved130[0x244 - 0x130];
    unsigned int m_field244;      // +0x244 (and +0x130 state, declared
                                    // separately below to keep this
                                    // struct's own field order readable)
};

struct GameRecord17State
{
    char m_reserved000[0x130];
    int m_state;                // +0x130
};

//  0x6F65ADB0 - already reconstructed for real (not a redirect) in
//  w3gsaction_tier8.cpp: FieldTarget::ResetField(void* buf).
struct FieldTarget
{
    void ResetField(void*);
};

//  Slot 0xA0 of the +0x58 sub-object's own vtable - the same call shape
//  w3gsaction_tier13.cpp's own 0x6F67AB30 already dispatches (5 stack
//  args, thiscall cleanup by construction). No committed vtable doc
//  names this class here either.
typedef void (__thiscall *NotifyActionFn)(void*, unsigned int, unsigned int, void*, void*, unsigned int);

long __fastcall InterlockedIncrementAt(volatile long* target);

#include "cdatastore.h"
#include "savegame.h"

//  0x6F684D00 - the field reader FieldValidator_6F675190 drives: the same
//  word-length-capped field SaveGameReadSlotRecord reads
//  (src/Game/savegameslotrecordread.cpp, shared with w3gsaction_tier8.cpp's
//  own FieldValidator_6F675450 - see that file's own comment for why the
//  address is declared `extern` rather than redefined), followed by a
//  fixed one-byte field at `out+0x82` (ReadByte) and a fixed 0x10-byte raw
//  block at `out+0x83` (ReadRawAlias, folded straight in here rather than
//  through the tiny sub_6F6508C0 wrapper tier8.cpp's own
//  NetMsgIoField_6F6840E0 comment already documents as nothing but that
//  one call).  `retn` bare - __fastcall(store, out).  Not part of this
//  batch's own worklist but needed as FieldValidator_6F675190's real
//  callee.
extern CDataStore* __fastcall SaveGameReadSlotRecord(CDataStore*, Net::GAMESETUP_SLOTRECORD*);
static CDataStore* __fastcall NetMsgIoField_6F684D00(CDataStore* store, void* out)
{
    SaveGameReadSlotRecord(store, (Net::GAMESETUP_SLOTRECORD*)out);
    store->ReadByte((unsigned char*)out + 0x82);
    store->ReadRawAlias((char*)out + 0x83, 0x10);
    return store;
}

//  0x6F675190 - fastcall(ecx=src, edx=declaredLength, stack out), retn 4.
//  Same base shape as w3gsaction_tier8.cpp's own FieldValidator family: a
//  real local `CDataStore`, one field reader call, success iff the reader
//  consumed exactly `declaredLength`.  Same unreproducible `/GS`-cookie
//  frame every sibling in this family carries.
int __fastcall FieldValidator_6F675190(unsigned int src, unsigned int declaredLength, void* out)
{
    CDataStore local;
    local.m_field4 = (void*)src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgIoField_6F684D00(&local, out);
    return local.m_readPos == local.m_field10;
}

//  0x6F67C4A0 - same callee w3gsaction_tier23.cpp already declares
//  (AllocNode_6F67C4A0); reused by forward declaration rather than
//  re-defined here.
extern "C" void* __fastcall AllocNode_6F67C4A0(void*, unsigned int, unsigned int, unsigned int);

//  0x6F5451B0 - same callee w3gsaction_tier23.cpp already declares, as
//  `RecordTailReceiver::CopyTail` (thiscall(this=dst)+1 stack arg src) -
//  a free `__thiscall` function is an MSVC error (C3865), so that file
//  spells this as a member of a one-off local receiver type; forward-
//  declared here the same way rather than re-defined.
struct RecordTailReceiver
{
    void CopyTail(void* src);
};

//  0x6F67A960 - same callee w3gsaction_tier13.cpp already declares with
//  this exact 2-argument signature; reused by forward declaration
//  rather than re-defined here.
extern "C" void __fastcall Sub_6F67A960(void*, unsigned int);

//  0x6F673710 - trivial: zero one field on the record. Reconstructed
//  for real (not a redirect) under two other names elsewhere in src/
//  already (trivialaccessors_06.cpp's ClearField0x16C_6F673710,
//  w3gsaction_tier3.cpp's ClearRecordField16C), and as a genuine
//  out-of-line redirect under two more (w3gsaction_tier15.cpp's
//  ClearField0x16C_6F680620, w3gsaction_tier23.cpp's
//  ClearField0x16C_6F67CE30) - this call site keeps its own copy of
//  the same redirect for the same reason those do: a real out-of-line
//  `call` in the shipped body here too.
extern "C" __declspec(naked) void __fastcall ClearField0x16C_6F67D040(void*)
{
    __asm { mov eax, 0x6F673710 }
    __asm { jmp eax }
}

//  0x6F67D040 - case (see w3gsactionhandler2.cpp). On a record match
//  whose own state < 3 and own +0x244 == 0, and whose own +0x10C
//  bounds the field validator's parsed id byte (>= 1): stamps self's
//  own +0x90/+0x94/+0x98/+0x9C/+0xA0..+0xAC/+0xB4 fields from the
//  validator's own buffer and the hidden fourth argument's own +0x10,
//  links self into the record's own +0x114 intrusive list
//  (TSExplicitList::LinkToHead), bumps the record's own +0x12C counter
//  and self's own +0x10 refcount, allocates a consumer node and copies
//  self's tail bytes into it, advances the record's own state to 4,
//  dispatches through the record's own +0x58 sub-object's vtable slot
//  0xA0, resets the validator's own buffer via FieldTarget::ResetField
//  and clears the record's own +0x16C field, reporting status 2. Every
//  other outcome reports 0/1 as usual for this batch.
int __stdcall W3GSAction2_6F67D040(void* self, unsigned int arg4, unsigned int arg8)
{
    void* hiddenArg4;
    __asm { mov hiddenArg4, edx }

    unsigned char buf[0x98];
    *(unsigned short*)buf = 0;

    if (!FieldValidator_6F675190(arg4, arg8, buf))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    unsigned int status = 0;
    if (key)
    {
        g_netClientRecordTableGuard.Wait(1);

        GameRecord17* rec = (GameRecord17*)MaskedPositive(g_netClientRecordListHead);
        while (rec && (void*)rec->m_recordKey != key)
            rec = (GameRecord17*)rec->m_nextRecord;

        GameRecord17State* recState = (GameRecord17State*)rec;
        if (rec && recState->m_state < 3 && rec->m_field244 == 0)
        {
            unsigned char idByte = buf[0x82];
            if (idByte >= 1 && idByte <= rec->m_field10C)
            {
                void* field58 = rec->m_field58;

                *(unsigned int*)((char*)self + 0x90) = *(unsigned int*)(buf + 0x83);
                *(unsigned int*)((char*)self + 0x94) = *(unsigned int*)(buf + 0x87);
                *(unsigned int*)((char*)self + 0x98) = *(unsigned int*)(buf + 0x8B);
                *(unsigned int*)((char*)self + 0x9C) = *(unsigned int*)(buf + 0x8F);

                unsigned int* extra = (unsigned int*)((char*)hiddenArg4 + 0x10);
                *(unsigned int*)((char*)self + 0xA0) = extra[0];
                *(unsigned int*)((char*)self + 0xA4) = extra[1];
                *(unsigned int*)((char*)self + 0xA8) = extra[2];
                *(unsigned int*)((char*)self + 0xAC) = extra[3];

                *(unsigned char*)((char*)self + 0xB4) = idByte;

                rec->m_actionList.LinkToHead((GameActionNode2*)self);

                rec->m_field12C += 1;
                InterlockedIncrementAt((volatile long*)((char*)self + 0x10));

                void* newNode = AllocNode_6F67C4A0((char*)rec + 0x120, 2, 0, 0);
                *((unsigned char*)newNode + 8) = *(unsigned char*)((char*)self + 0xB4);
                ((RecordTailReceiver*)((char*)newNode + 9))->CopyTail((char*)self + 0xB5);

                Sub_6F67A960(rec, 4);

                NotifyActionFn notify = ((NotifyActionFn*)(*(void**)field58))[0xA0 / 4];
                notify(field58, 1, idByte, (char*)self + 0xB5, (char*)rec + 0x5C, rec->m_recordKey);

                ((FieldTarget*)field58)->ResetField(buf);
                ClearField0x16C_6F67D040(rec);

                status = 2;
            }
        }

        g_netClientRecordTableGuard.SignalOne(1);
    }
    return (int)status;
}
