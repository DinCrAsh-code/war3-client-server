//============================================================================
//  batch-B-w3gsaction_thunks - three handlers (0x6F67AD90, 0x6F67AF30,
//  0x6F67B370) sharing the same guarded singly-linked record table
//  (unk_6FACFF90/dword_6FACFFA4/MaskedPositive) w3gsaction_tier3.cpp's own
//  entries already reconstruct, each parsing into a local buffer through
//  its own out-of-scope field validator first.
//
//  All three carry a `/GS` stack-cookie frame in the shipped body (the
//  `mov eax, g_securityCookie` / `xor eax, esp` / store ahead of the local
//  buffer, and the matching `xor ecx, esp` / `call SecurityCheckCookie`
//  pair at every exit) that this repo's fixed `/GS-` build can never emit,
//  no matter how the local is spelled - the same "unreproducible frame
//  instructions, IDENTICAL past them" shape docs/msvc-vc8-idioms.md already
//  catalogues for the `__except_handler4`-shaped SEH frame
//  w3gsaction_parsers.cpp's own header documents, just for a plain cookie
//  rather than full SEH. Past that fixed, unavoidable gap every one of
//  these three is otherwise a faithful, instruction-for-instruction
//  transliteration.
//============================================================================
#include "cdatastore.h"
#include "savegame.h"

extern "C" void* __cdecl memcpy(void*, const void*, unsigned int);

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

//  Same guarded-table record as w3gsaction_tier3.cpp's own GameRecord /
//  w3gsaction_tier7.cpp's own GameRecord2, extended with the two more
//  fields this trio reads: +0x274 (a sub-object `this` for 0x6F67B370's
//  own trigger) alongside +0x58 (shared with w3gsaction_tier7.cpp's own
//  GameRecord2::m_field58) and +0x130 (shared with GameRecord2::m_state).
struct GameRecord3
{
    char m_reserved000[0x8];
    void* m_nextRecord;        // +0x08
    unsigned int m_recordKey;  // +0x0C
    char m_reserved010[0x58 - 0x10];
    void* m_field58;           // +0x58
    char m_reserved05C[0x130 - 0x5C];
    int  m_state;               // +0x130
    char m_reserved134[0x274 - 0x134];
    void* m_field274;           // +0x274
};

//  0x6F684D60 / 0x6F443720 - the field reader FieldValidator_6F675450
//  drives: read a word length prefix (cap 0x80, unsigned), then that many
//  raw bytes right after it into `out+2` through CDataStore::ReadRawAlias -
//  `out[0..1]` gets the length itself, `out[2..]` the payload.  A length
//  over the cap invalidates the stream the same way every other
//  length-prefixed reader in this repo does, and the length word is
//  zeroed on that same overrun path.  0x6F443720 is already reconstructed
//  under a different (better) name - SaveGameReadSlotRecord
//  (src/Game/savegameslotrecordread.cpp), whose own `GAMESETUP_SLOTRECORD`
//  out-param (`{unsigned short m_length; char m_data[0x80];}`, savegame.h)
//  is byte-for-byte this validator's own `out[0..1]`/`out[2..]` layout -
//  declared `extern` and reused rather than a second, independent
//  redefinition under a new name, the "one shipped address, two names"
//  defect class thunk_inventory.py's own docs/notes/thunk-removal-pass.md
//  flags.  Not part of this batch's own worklist (only the
//  FieldValidator_* wrappers named there reach it) but needed as their
//  real callee - and FieldValidator_6F675190's own callee too
//  (w3gsaction_tier24.cpp, through that file's own NetMsgIoField_6F684D00),
//  a second genuine call site to the same address, not a duplicate body.
extern CDataStore* __fastcall SaveGameReadSlotRecord(CDataStore*, Net::GAMESETUP_SLOTRECORD*);
static CDataStore* __fastcall NetMsgIoField_6F684D60(CDataStore* store, void* out)
{
    SaveGameReadSlotRecord(store, (Net::GAMESETUP_SLOTRECORD*)out);
    return store;
}

//  0x6F675450 - retn 4 (fastcall ecx/edx + one stack buffer pointer).
int __fastcall FieldValidator_6F675450(unsigned int src, unsigned int declaredLength, void* out)
{
    CDataStore local;
    local.m_field4 = (void*)src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgIoField_6F684D60(&local, out);
    return local.m_readPos == local.m_field10;
}

//  0x6F6596B0 - not part of this batch's own worklist: reached only
//  through FieldTarget::ResetField below.  `retn 8`, confirmed from its
//  own trailing bytes (`C2 08 00`) - __thiscall(this) plus a literal tag
//  and a pointer to the just-copied local buffer.  A real
//  __except_handler4-shaped SEH frame (`push -1` opening the body)
//  unreproducible at this build's fixed /GS-.
class FieldTargetDispatch
{
public:
    void Post(int tag, void* buf);
};
__declspec(naked) void FieldTargetDispatch::Post(int, void*)
{
    __asm { mov eax, 0x6F6596B0 }
    __asm { jmp eax }
}

//  0x6F65ADB0 - copy the length-prefixed field FieldValidator_6F675450's
//  own reader just parsed (a word length at `buf[0..1]`, up to 0x80 bytes
//  of payload at `buf[2..]`) into a fixed 0x19-byte local and hand it to
//  this target's own FieldTargetDispatch::Post under tag 0x19.  Carries the
//  same unreproducible /GS- cookie frame every sibling in this file does.
//  A free function cannot be declared `__thiscall` (only a native member
//  function may), so this is a member of a tiny placeholder type reached
//  through `record->m_field58`, the same pattern w3gsaction_tier3.cpp's own
//  CopyTarget::RawCopyIn already establishes.
struct FieldTarget
{
    void ResetField(void* buf);
};
void FieldTarget::ResetField(void* buf)
{
    unsigned char local[0x19];
    unsigned short len = *(unsigned short*)buf;
    *(unsigned short*)local = len;
    memcpy(local + 2, (char*)buf + 2, len);
    ((FieldTargetDispatch*)this)->Post(0x19, local);
}

//  0x6F67AD90 - case (see w3gsactionhandler2.cpp). Parse into a local
//  buffer whose first dword the shipped body explicitly pre-zeroes (the
//  validator only conditionally fills it), then on a record match: state<6
//  runs the not-in-scope Sub_6F65ADB0(record->m_field58, &buf) and reports
//  status 2, state>=6 reports status 1 with no trigger.
int __stdcall W3GSAction2_6F67AD90(void* self, unsigned int arg4, unsigned int arg8)
{
    unsigned char buf[0x84];   // sub esp,0x88 minus the unreproducible 4-byte cookie slot
    *(unsigned int*)buf = 0;

    if (!FieldValidator_6F675450(arg4, arg8, buf))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    unsigned int status = 0;
    if (key)
    {
        g_netClientRecordTableGuard.Wait(0);

        GameRecord3* rec = (GameRecord3*)MaskedPositive(g_netClientRecordListHead);
        while (rec && (void*)rec->m_recordKey != key)
            rec = (GameRecord3*)rec->m_nextRecord;

        if (rec)
        {
            if (rec->m_state < 6)
            {
                ((FieldTarget*)rec->m_field58)->ResetField(buf);
                status = 2;
            }
            else
            {
                status = 1;
            }
        }

        g_netClientRecordTableGuard.SignalOne(0);
    }
    return (int)status;
}

//  0x6F684DF0 / 0x6F651CF0 - the field reader FieldValidator_6F675660
//  drives: the byte-length twin of SaveGameReadSlotRecord above (cap 0x60,
//  a one-byte prefix through CDataStore::ReadByte rather than ReadWord,
//  the payload starting at `out+1` instead of `out+2`) - same overrun/zero
//  shape otherwise.  0x6F651CF0 is already reconstructed under a different
//  name too - TurnsSyncFieldWrite_6F651CF0 (netevent_dispatch.cpp; the
//  name is misleading, every call it makes is a read, not a write, per
//  that file's own comment) - declared `extern` and reused for the same
//  "one shipped address, two names" reason as SaveGameReadSlotRecord
//  above.  Not part of this batch's own worklist but needed as
//  FieldValidator_6F675660's real callee.
extern CDataStore* __fastcall TurnsSyncFieldWrite_6F651CF0(void* store, void* dest);
static CDataStore* __fastcall NetMsgIoField_6F684DF0(CDataStore* store, void* out)
{
    TurnsSyncFieldWrite_6F651CF0(store, out);
    return store;
}

//  0x6F675660 - retn 4, same shape as FieldValidator_6F675450.
int __fastcall FieldValidator_6F675660(unsigned int src, unsigned int declaredLength, void* out)
{
    CDataStore local;
    local.m_field4 = (void*)src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgIoField_6F684DF0(&local, out);
    return local.m_readPos == local.m_field10;
}

//  0x6F659C50 - not part of this batch's own worklist: reached only
//  through SplitBufferTarget::WriteSplit below.  `retn 8`, confirmed from
//  its own trailing bytes (`C2 08 00`) - __thiscall(this) plus a literal
//  tag and a pointer to the just-copied local buffer.  Same unreproducible
//  SEH-frame shape as FieldTargetDispatch::Post above.
class SplitTargetDispatch
{
public:
    void Post(int tag, void* buf);
};
__declspec(naked) void SplitTargetDispatch::Post(int, void*)
{
    __asm { mov eax, 0x6F659C50 }
    __asm { jmp eax }
}

//  0x6F65B030 - copy the byte-length-prefixed field FieldValidator_6F675660
//  just parsed (a one-byte length at `tail[-1]`/`leadByte`, up to 0x60
//  bytes of payload at `tail[0..]`) into a fixed 0x22-byte local and hand
//  it to this target's own SplitTargetDispatch::Post under tag 0x22.
//  Carries the same unreproducible /GS- cookie frame every sibling in this
//  file does.  A free function cannot be declared `__thiscall`, so this is
//  a member of a tiny placeholder type reached through `record->m_field58`,
//  the same pattern FieldTarget::ResetField above already establishes.
struct SplitBufferTarget
{
    void WriteSplit(void* tail, unsigned int leadByte);
};
void SplitBufferTarget::WriteSplit(void* tail, unsigned int leadByte)
{
    unsigned char local[0x22];
    local[0] = (unsigned char)leadByte;
    memcpy(local + 1, tail, leadByte);
    ((SplitTargetDispatch*)this)->Post(0x22, local);
}

//  0x6F67AF30 - case (see w3gsactionhandler2.cpp). Parse into a local
//  buffer whose first byte the shipped body explicitly pre-zeroes, then -
//  unconditionally on any record match, no state gate - splits the buffer
//  at its own first byte into the not-in-scope
//  SplitBufferTarget::WriteSplit(record->m_field58, &buf[1], buf[0]) and
//  reports status 2.
int __stdcall W3GSAction2_6F67AF30(void* self, unsigned int arg4, unsigned int arg8)
{
    unsigned char buf[0x64];   // sub esp,0x68 minus the unreproducible 4-byte cookie slot
    buf[0] = 0;

    if (!FieldValidator_6F675660(arg4, arg8, buf))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    unsigned int status = 0;
    if (key)
    {
        g_netClientRecordTableGuard.Wait(0);

        GameRecord3* rec = (GameRecord3*)MaskedPositive(g_netClientRecordListHead);
        while (rec && (void*)rec->m_recordKey != key)
            rec = (GameRecord3*)rec->m_nextRecord;

        if (rec)
        {
            ((SplitBufferTarget*)rec->m_field58)->WriteSplit(buf + 1, buf[0]);
            status = 2;
        }

        g_netClientRecordTableGuard.SignalOne(0);
    }
    return (int)status;
}

//  0x6F6840E0 - the field reader FieldValidator_6F675A80 drives: a fixed
//  25-byte (0x19) record with no length prefix or overrun check at all -
//  a byte through ReadByte, 0x10 raw bytes through ReadRawAlias (via the
//  tiny wrapper sub_6F6508C0, folded in here since its own dump is nothing
//  but that one call), then two back-to-back dwords through ReadDword.
//  Not part of this batch's own worklist but needed as
//  FieldValidator_6F675A80's real callee.
static CDataStore* __fastcall NetMsgIoField_6F6840E0(CDataStore* store, void* out)
{
    unsigned char* dst = (unsigned char*)out;
    store->ReadByte(dst);
    store->ReadRawAlias(dst + 1, 0x10);
    store->ReadDword((unsigned int*)(dst + 0x11));
    store->ReadDword((unsigned int*)(dst + 0x15));
    return store;
}

//  0x6F675A80 - retn 4, same shape as FieldValidator_6F675450 but with no
//  overrun check of its own (NetMsgIoField_6F6840E0's own reads carry that
//  guard internally, per CDataStore::ReadByte/ReadRawAlias/ReadDword's own
//  shared EnsureReadable path) - matches the dump, which has no `m_readPos
//  == m_field10` compare in this validator at all past the reader call.
int __fastcall FieldValidator_6F675A80(unsigned int src, unsigned int declaredLength, void* out)
{
    CDataStore local;
    local.m_field4 = (void*)src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgIoField_6F6840E0(&local, out);
    return local.m_readPos == local.m_field10;
}

//  0x6F674480 - look this record's own hero-slot table (`ecx+0x14`
//  count, `ecx+0x18` base, 8 bytes a slot) up by the raw byte at `edx[0]`
//  from the end backwards, falling back to slot -1 (one before the base)
//  when nothing matched or the table was empty; then, if that landed on a
//  free slot (`base[slot*7 + 0x30] == 0`, a parallel byte array 7 bytes
//  past each 8-byte slot header), claims it (writes 1 there) and copies
//  five wire fields (a dword, a dword, a dword, a dword, a byte - `edx`'s
//  own +1/+5/+9/+0xD/+0x11/+0x15 offsets) into the slot's own two
//  trailing dword pairs at `+0x18`/`+0x28`.  Returns 1 only on that claim;
//  0 for a null `this`, an unmatched/full table, or an already-claimed
//  slot.  `retn` bare - __fastcall(ecx=record, edx=fields).
int __fastcall Sub_6F674480(void* self, void* edx)
{
    if (self == 0)
        return 0;

    struct HeroSlot { unsigned int m_field0; unsigned int m_field4; };
    unsigned char* fields = (unsigned char*)edx;

    int count = *(int*)((char*)self + 0x14);
    HeroSlot* base = *(HeroSlot**)((char*)self + 0x18);
    char* flags = (char*)base + 0x30;

    int slot = count - 1;
    if (count != 0)
    {
        while (flags[slot] != fields[0] && --slot >= 0)
            ;
    }
    else
    {
        slot = -1;
    }

    if (slot < 0)
        return 0;

    char* claimed = (char*)*(HeroSlot**)((char*)self + 0x18) + slot * 7 + 0x30;
    if (*claimed != 0)
        return 0;

    *claimed = 1;

    HeroSlot* dest18 = (HeroSlot*)((char*)*(void**)((char*)self + 0x18) + slot * 8 + 0x18);
    dest18->m_field0 = *(unsigned int*)(fields + 1);
    dest18->m_field4 = *(unsigned int*)(fields + 5);
    unsigned int* dest18b = (unsigned int*)((char*)dest18 + 8);
    dest18b[0] = *(unsigned int*)(fields + 9);
    dest18b[1] = *(unsigned int*)(fields + 0xD);

    HeroSlot* dest28 = (HeroSlot*)((char*)*(void**)((char*)self + 0x18) + slot * 8 + 0x28);
    dest28->m_field0 = *(unsigned int*)(fields + 0x11);

    unsigned int* dest2c = (unsigned int*)((char*)*(void**)((char*)self + 0x18) + slot * 8 + 0x2C);
    *dest2c = *(unsigned int*)(fields + 0x15);

    return 1;
}

//  0x6F67B370 - case (see w3gsactionhandler2.cpp). Parse into a local
//  buffer with no explicit pre-zero (the validator fills it unconditionally
//  in this one), then on a state==9 EXACT match (not a threshold) calls
//  Sub_6F674480(record->m_field274, &buf) and reports status 2 only if
//  that call itself reports nonzero; any other outcome (no record, wrong
//  state, or a zero result from the call) leaves status 0.
int __stdcall W3GSAction2_6F67B370(void* self, unsigned int arg4, unsigned int arg8)
{
    unsigned char buf[0x1C];   // sub esp,0x20 minus the unreproducible 4-byte cookie slot

    if (!FieldValidator_6F675A80(arg4, arg8, buf))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    unsigned int status = 0;
    if (key)
    {
        g_netClientRecordTableGuard.Wait(1);

        GameRecord3* rec = (GameRecord3*)MaskedPositive(g_netClientRecordListHead);
        while (rec && (void*)rec->m_recordKey != key)
            rec = (GameRecord3*)rec->m_nextRecord;

        if (rec && rec->m_state == 9 && Sub_6F674480(rec->m_field274, buf))
            status = 2;

        g_netClientRecordTableGuard.SignalOne(1);
    }
    return (int)status;
}
