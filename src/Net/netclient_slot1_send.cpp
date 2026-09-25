//============================================================================
//  Net::NetClient vtable slot 1 (0x6F67A800) and its own shared framing
//  callee (0x6F676C70) - networking wave 9, agent-networking-wave3-2026-08-29.
//
//  Shares the record-table-walk shape every other NetClient slot in this
//  directory establishes (g_netClientRecordTableGuard/g_netClientRecordListHead,
//  MaskedPositive on the list head only, a plain `>0` continue test on each
//  node's own `+0x8` next pointer), but its own per-byte body is new: for
//  each byte in the caller's own `srcBytes[0..count)`, it walks a SECOND
//  per-record table (`rec->m_tableBase`/`rec->m_tableHead`, the same
//  +0x114/+0x11C rebased-next scheme netclient_notifyevent_0x22.cpp's own
//  header documents in full) looking for a provider whose own `+0xB4` tag
//  byte matches. Three outcomes, matching the disassembly's own three-way
//  branch exactly:
//   - the byte is the literal sentinel 0xFF - skip the table search
//     entirely and treat it like "no match";
//   - a distinct provider (not the record's own primary, `rec->m_provider`)
//     is found and its own `+0x3C` target is non-null - send that ONE byte
//     immediately, as its own single-element frame, straight to that
//     provider's target (tag 0x34), and do NOT add it to the accumulated
//     buffer;
//   - anything else (no match, table empty, matched entry IS the primary
//     provider, or its target is null) - accumulate the byte into a local
//     16-byte buffer instead.
//  After the whole input has been scanned, if anything was accumulated,
//  one more frame (tag 0x28) carries the whole accumulated buffer to the
//  record's own primary provider's target.
//
//  Both frames share one out-of-line builder (0x6F676C70, below): open a
//  local CDataStoreScratch, write {0xF7, tag, wordLength}, then the
//  length-prefixed record via WriteLengthPrefixedRecord (sub_6F684C90,
//  cdatastorescratch_writers.cpp), destroy the scratch object, and dispatch
//  through vtable slot 11 - the same {0xF7,tag}-then-DispatchVtableSlot11
//  frame shape the notify family and NetClientBuildARelaySelf::BuildAndSend
//  (netclient_buildarelay.cpp) both already establish, just built directly
//  from explicit (tag, idByte, bytes, count) arguments here instead of a
//  caller-supplied callback.
//
//  0x6F676C70 is not a plain thiscall member: `this` (ecx, the frame's own
//  send target) AND a one-byte tag (edx, the low byte of a real fastcall
//  second argument, matching `mov dl, 28h`/`mov dl, 34h` right before each
//  call site) both arrive in registers - the ordinary MSVC __fastcall
//  convention for a `(void*, unsigned char, ...)` parameter list, not a
//  hand-picked one like ConditionVariable::NotifyOne's own `esi` convention.
//  Its own local record layout (a fixed 16-byte data buffer preceded by a
//  count byte and followed one byte later by a fixed "id" byte at a
//  constant +0x11 offset, written *before* the data - matching this file's
//  own second call site, which packs a single immediate byte into that
//  same +0x11-relative shape) is `WriteLengthPrefixedRecord`'s own `src`
//  argument.
//
//  The word length field itself is computed, not copied: `count + (word
//  truncation of) lenBase + 6`, then combined again with the outer
//  function's own `paramC` argument (full 32-bit, not truncated) to build
//  the CDataStoreScratch constructor's own two opaque `field18`/`field1C`
//  arguments - transcribed literally off the listing (`add dx, bp` is a
//  genuine 16-bit truncating add, `add eax, ebp` right after is a genuine
//  32-bit one) since nothing in this call tree establishes what either
//  field actually represents.
//============================================================================
#include "cdatastorescratch.h"

struct ConditionVariable
{
    void Wait(unsigned int value);
    void SignalOne(unsigned int value);
};

//  netclient_recvappend.cpp - the same global guard and list head every
//  other NetClient record-table walk in this directory uses.
extern ConditionVariable g_netClientRecordTableGuard;   // unk_6FACFF90
extern int               g_netClientRecordListHead;     // dword_6FACFFA4

void __fastcall DispatchVtableSlot11(void* self, int edxArg, unsigned int arg0);
CDataStoreScratch* __fastcall WriteLengthPrefixedRecord(CDataStoreScratch* self, const unsigned char* src);

//  A masked field is only ever a positive object pointer or a poisoned
//  non-positive encoding - the same idiom every sibling file in this
//  directory redeclares locally.
static void* MaskedPositive(int v)
{
    return (v > 0) ? (void*)v : 0;
}

//  The fixed local record `sub_6F676C70` builds and hands to
//  WriteLengthPrefixedRecord: a leading count byte, up to 16 data bytes,
//  and one more fixed byte at +0x11 - the exact layout that function's own
//  header documents.
struct FrameLocalRecord
{
    unsigned char m_count;      // +0x00
    unsigned char m_data[0x10]; // +0x01
    unsigned char m_idTag;      // +0x11
};

//----------------------------------------------------------------------------
//  0x6F676C70 - see the file header above.
//----------------------------------------------------------------------------
void __fastcall NetClientBuildFrame(void* target, unsigned char tag, unsigned char idByte,
                                     const unsigned char* srcBytes, unsigned int count,
                                     unsigned int paramC, unsigned int lenBase)
{
    //  The disassembly spills every register argument to its own stack
    //  slot in strict left-to-right parameter order (`edx`/tag first, then
    //  `arg_8`/count, `arg_C`/paramC, `arg_10`/lenBase, and only then
    //  `ecx`/target) before doing anything else with any of them - matched
    //  here by reading each parameter into its own local in that same
    //  order, rather than using the parameters in place, which is what
    //  gets this build's own register allocator to spill them in the same
    //  sequence.
    unsigned char savedTag = tag;
    unsigned char localCount = (unsigned char)count;
    unsigned int  base = paramC;
    unsigned int  baseLen = lenBase;
    void* self = target;

    FrameLocalRecord record;
    record.m_count = localCount;
    if (record.m_count != 0)
    {
        //  Written as a do-while that reloads `record.m_count` from memory
        //  each pass (matching the disassembly's own `mov dl, var_24`
        //  reload every iteration) rather than a `for` loop over a
        //  register-cached trip count - the latter is exactly the shape
        //  this build's /O2 recognises and folds into a `memcpy` call,
        //  which the target does not have (a real, inlined byte loop).
        unsigned char i = 0;
        do
        {
            record.m_data[i] = srcBytes[i];
            i = i + 1;
        } while (i < record.m_count);
    }

    unsigned short frameLen = (unsigned short)(record.m_count + (unsigned short)baseLen + 6);
    unsigned int field18 = base - frameLen + baseLen;
    record.m_idTag = idByte;

    CDataStoreScratch writer((void*)field18, (void*)(unsigned int)frameLen);

    writer.WriteByte(0xF7);
    writer.WriteByte(savedTag);
    writer.WriteWord(frameLen);

    WriteLengthPrefixedRecord(&writer, (const unsigned char*)&record);

    DispatchVtableSlot11(self, (int)field18, frameLen);
}

//  Only the fields this slot itself reads; see the sibling files' own
//  header comments for why these stay raw offset casts rather than one
//  shared struct.
struct NetClientRecordFieldsSlot1
{
    char m_reserved000[0xC];
    unsigned int m_recordKey;  // +0x0C
    char m_reserved010[0x114 - 0x10];
    void* m_tableBase;         // +0x114
    char m_reserved118[0x11C - 0x118];
    int   m_tableHead;         // +0x11C
    char m_reserved120[0x130 - 0x120];
    int   m_state;             // +0x130
    char m_reserved134[0x148 - 0x134];
    void* m_provider;          // +0x148 - the record's own primary provider
};

struct NetClientProviderFieldsSlot1
{
    char m_reserved00[0x3C];
    void* m_target;            // +0x3C
    char m_reserved40[0xB4 - 0x40];
    unsigned char m_tagByte;   // +0xB4
};

//  The local accumulate buffer the "no direct-send match" path fills.
//  `m_capacity` is written unconditionally (even when the outer loop below
//  never runs - the shipped code's own `mov [esp+..], 10h` sits ahead of
//  the `count == 0` early-out) and never read again in this function; kept
//  as a real field rather than dropped, since the write is real and this
//  build's own `/W4` would otherwise flag an unused local.
struct LocalMatchBuffer
{
    unsigned int m_capacity;
    unsigned char m_data[0x10];
};

struct NetClientDispatchSelf
{
    void SendMatchedFrames(const unsigned char* srcBytes, unsigned int count,
                            unsigned int paramC, unsigned int lenBase);
};

//----------------------------------------------------------------------------
//  0x6F67A800 - Net::NetClient vtable slot 1.
//----------------------------------------------------------------------------
void NetClientDispatchSelf::SendMatchedFrames(const unsigned char* srcBytes, unsigned int count,
                                               unsigned int paramC, unsigned int lenBase)
{
    void* self = this;
    void* key = *(void**)((char*)self + 4);
    if (!key)
        return;

    g_netClientRecordTableGuard.Wait(0);

    NetClientRecordFieldsSlot1* rec = (NetClientRecordFieldsSlot1*)MaskedPositive(g_netClientRecordListHead);
    bool found = false;
    if (rec)
    {
        do
        {
            if (*(void**)((char*)rec + 0xC) == key)
            {
                found = true;
                break;
            }
            rec = (NetClientRecordFieldsSlot1*)*(void**)((char*)rec + 0x8);
        } while ((int)rec > 0);
    }

    if (found && rec->m_state >= 4 && rec->m_provider != 0)
    {
        LocalMatchBuffer buffer;
        unsigned int matched = 0;
        buffer.m_capacity = 0x10;

        if (count != 0)
        {
            for (unsigned int i = 0; i < count; ++i)
            {
                unsigned char b = srcBytes[i];
                bool sentDirect = false;

                if (b != 0xFF)
                {
                    NetClientProviderFieldsSlot1* entry =
                        (NetClientProviderFieldsSlot1*)MaskedPositive(rec->m_tableHead);
                    while (entry && entry->m_tagByte != b)
                    {
                        int next = *(int*)((char*)rec->m_tableBase + (int)entry + 4);
                        entry = (NetClientProviderFieldsSlot1*)((next > 0) ? (void*)next : 0);
                    }

                    if (entry && (void*)entry != rec->m_provider && entry->m_target)
                    {
                        unsigned char immediate = b;
                        NetClientProviderFieldsSlot1* primary =
                            (NetClientProviderFieldsSlot1*)rec->m_provider;
                        NetClientBuildFrame(entry->m_target, 0x34, primary->m_tagByte,
                                            &immediate, 1, paramC, lenBase);
                        sentDirect = true;
                    }
                }

                if (!sentDirect)
                {
                    buffer.m_data[matched] = b;
                    matched += 1;
                }
            }

            if (matched != 0)
            {
                NetClientProviderFieldsSlot1* primary = (NetClientProviderFieldsSlot1*)rec->m_provider;
                if (primary->m_target)
                {
                    NetClientBuildFrame(primary->m_target, 0x28, primary->m_tagByte,
                                        buffer.m_data, matched, paramC, lenBase);
                }
            }
        }
    }

    g_netClientRecordTableGuard.SignalOne(0);
}
