//============================================================================
//  The record-shape machinery CNetEventTypeDispatcher::ReplayAckDispatch
//  (0x6F54EB00, netevent_replaystream.cpp) dispatches into: the broadcast-
//  id clamp, the ~14 per-record-type echo handlers, the ~9 dispatcher-level
//  record decoders, and the raw-turn-payload submit tail.  Kept in its own
//  translation unit, apart from the dispatcher itself, purely so `/Ob2`
//  cannot inline any of these into the dispatcher's own listing the way
//  the shipped, separately-compiled module never did - every one of these
//  is a genuine `call` in the dispatcher's own dump, not an inlined body
//  (CLAUDE.md, "one translation unit per original module";
//  replaypayloadbuffer.cpp's own header note is the same reasoning for
//  PreparePayloadBuffer).
//
//  batch-C-netevent_replaystream (networking_desync_closure_worklist.json)
//  un-thunked all of this.  Reading the real dispatcher call sites
//  (0x6F54EB00's own dump) settled two things the previous session's
//  naked-thunk placeholders got wrong, both corrected here:
//
//    * the ~14 per-record-type handlers are NOT "echo one lobby/session
//      record a second time" leaves - each builds its own local
//      `CDataStore`-shaped record from the freshly decoded one and
//      forwards it through the shared tail below.  Real dispatch logic,
//      not a passthrough.
//    * every one of the ~9 `RECORD_CTOR_THUNK` declarations had the
//      caller-supplied record pointer arriving in `edx` silently marked
//      "unused, dead" - exactly the defect class
//      docs/notes/thunk-removal-pass.md's own first pass already found
//      once ("a declaration that hid a second argument arriving in
//      edx").  `edx` is read by every one of these nine: it is the
//      *destination* record the caller wants each field read into.
//    * the ~14 handlers' own 3 stack arguments were declared in the
//      wrong order (`zero, record, id`) - the real push order, read off
//      every one of the dispatcher's own call sites, is `id, record,
//      zero` (`arg_0` is the clamped broadcast id, `arg_4` the record,
//      `arg_8` the always-0 trailer nothing reads).
//============================================================================
#include "netdata.h"
#include "cdatastore.h"
#include "cdatastorescratch.h"
#include "storm.h"       // SMemFree (Storm_403) - NotifyTurnPayload/Alt's own tail
#include "gamecontext.h" // GetThreadLocalSlot (0x6F4C34D0)
#include "netdataeventqueue.h" // AllocEventSlot_6F549B80

void __cdecl nullsub_1(const char*, ...);

//  netmsgio_651.cpp already reconstructed these four addresses under its
//  own family of names - redeclared here for extern linkage rather than
//  re-thunked/re-defined under a second name (funcmap.py, thunk_inventory.
//  py's own "one shipped address, two names" defect class).  0x6F651610/
//  1630/1650 are three of that file's `NetMsgWriteDword_*` /OPT:ICF-less
//  duplicates, each just `self->WriteDword(*src)`; 0x6F651770 is
//  `NetMsgWriteDwordPair_6F651770`, two adjacent dwords.
extern CDataStoreScratch* __fastcall NetMsgWriteDword_6F651610(CDataStoreScratch* self, const unsigned int* src);
extern CDataStoreScratch* __fastcall NetMsgWriteDword_6F651630(CDataStoreScratch* self, const unsigned int* src);
extern CDataStoreScratch* __fastcall NetMsgWriteDword_6F651650(CDataStoreScratch* self, const unsigned int* src);
extern CDataStoreScratch* __fastcall NetMsgWriteDwordPair_6F651770(CDataStoreScratch* self, const unsigned int* src);

//  0x6F650D40 - PreparePayloadBuffer.  Reconstructed in its own
//  translation unit (replaypayloadbuffer.cpp) for the same same-module
//  inlining reason this whole file exists.
extern void* __fastcall PreparePayloadBuffer_6F650D40(unsigned int requestedSize, unsigned int* outCapacity);

#define NAKED_THUNK(sig, addr)             \
    __declspec(naked) sig                  \
    {                                       \
        __asm { mov eax, addr }             \
        __asm { jmp eax }                   \
    }

//----------------------------------------------------------------------------
//  0x6F5375E0 - ClampBroadcastId.  `this`-only (ecx in, eax out), retn bare.
//  Clamp the record-type byte (or, from the SubmitDecodedTurnPayload path,
//  a raw record-type byte read straight off the stream) into either itself
//  (values below 0x40) or a single "unrecognised" sentinel, 0xFF.
//----------------------------------------------------------------------------
unsigned char __fastcall ClampBroadcastId_6F5375E0(unsigned int id)
{
    return ((unsigned char)id < 0x40) ? (unsigned char)id : (unsigned char)0xFF;
}

//----------------------------------------------------------------------------
//  The ~14 record-type handlers - `this` = self, 3 stack args (the clamped
//  broadcast id, the freshly decoded record, and an always-0 trailer
//  nothing here reads), retn 0Ch.  Each re-serializes the decoded record
//  into its own throwaway `CDataStore` (built from a size-class-14 pool
//  chunk borrowed with `requestedSize == 0`, i.e. "whatever the pool
//  already has checked in") via a handler-specific re-serializer this
//  session did not chase (out of scope - the mission is this file's own
//  dispatch shape, not the ~40-strong CNetEvent-constructor family
//  netevent_dispatch.cpp's own header already defers), then hands the
//  result to the shared publish tail.
//----------------------------------------------------------------------------

//  sub_6F652270/sub_6F652160 - two record-*field* serializers one level
//  below the ten re-serializers this batch (batch-I-playertable-and-bnet,
//  networking_desync_closure_worklist_wave2.json) un-thunks: a "write a
//  NUL-terminated string, then its own trailing length-prefixed raw blob"
//  field (WriteString/WriteByte/TailCallWriteRaw, all already
//  reconstructed - cdatastorescratch.h) and "three of those strings plus
//  two trailing dwords" respectively.  Neither is one of this batch's own
//  27 addresses, but both compile from methods this file already has, so
//  reconstructing them outright costs nothing and removes two more thunks
//  from the inventory rather than leaving them "out of scope" for no
//  reason (CLAUDE.md: that phrase is not a KEPT reason).  `this` =
//  destStore, `edx` = srcField, both retn bare.
CDataStoreScratch* __fastcall SerializeLenPrefixedField_6F652270(CDataStoreScratch* dst, void* srcField)
{
    unsigned char* src = (unsigned char*)srcField;
    dst->WriteString((const char*)src);
    unsigned char len = src[0x10];
    dst->WriteByte(len);
    dst->TailCallWriteRaw(src + 0x11, len);
    return dst;
}

CDataStoreScratch* __fastcall SerializeThreeStringsAndTwoDwords_6F652160(CDataStoreScratch* dst, void* srcField)
{
    unsigned char* src = (unsigned char*)srcField;
    dst->WriteString((const char*)src);
    dst->WriteString((const char*)(src + 0x20));
    dst->WriteString((const char*)(src + 0x30));
    dst->WriteDword(*(unsigned int*)(src + 0xB0));
    dst->WriteDword(*(unsigned int*)(src + 0xB4));
    return dst;
}

//  sub_6F652A70/sub_6F651D80 and sub_6F6517B0 - the *write*-side twins of
//  the two array-element helpers ReadFixedField16 (below) reads through:
//  "write a length-prefixed field, then a fixed 0x10-byte raw blob written
//  one byte at a time" and "write a byte plus two dwords" respectively.
//  Not one of this batch's 27 addresses either, but again built entirely
//  from already-reconstructed writers, so reconstructed rather than
//  thunked.  `this` = destStore, `edx` = entry; sub_6F651D80 takes one
//  stack arg (size) that its own body never actually reads (the count it
//  writes/loops on is the entry's own leading byte, not the caller-
//  supplied size - transcribed as-is, not "fixed", since the shipped
//  callee genuinely ignores it), retn 4; the other two retn bare.
void* __fastcall WriteRawBytesLoop_6F651D80(void* destStoreVoid, void* dest, unsigned int /*size - unused by the shipped body*/)
{
    CDataStoreScratch* dst = (CDataStoreScratch*)destStoreVoid;
    unsigned char* d = (unsigned char*)dest;
    unsigned char count = d[0];
    dst->WriteByte(count);
    for (unsigned int i = 0; i < count; i++)
        dst->WriteByte(d[4 + i]);
    return destStoreVoid;
}

void* __fastcall WriteRecordSlot_6F652A70(void* destStoreVoid, void* entry)
{
    CDataStoreScratch* dst = (CDataStoreScratch*)destStoreVoid;
    unsigned char* e = (unsigned char*)entry;
    unsigned char len = e[0];
    dst->WriteByte(len);
    dst->TailCallWriteRaw(e + 1, len);
    WriteRawBytesLoop_6F651D80(destStoreVoid, e + 0x64, 0x10);
    return destStoreVoid;
}

void* __fastcall WriteRecordSlot_6F6517B0(void* destStoreVoid, void* entry)
{
    CDataStoreScratch* dst = (CDataStoreScratch*)destStoreVoid;
    unsigned char* e = (unsigned char*)entry;
    dst->WriteByte(e[0]);
    dst->WriteDword(*(unsigned int*)(e + 4));
    dst->WriteDword(*(unsigned int*)(e + 8));
    return destStoreVoid;
}

//  sub_6F652D20/sub_6F651E20 - "write a byte count, then that many
//  0x78-/0x0C-stride array entries through WriteRecordSlot_6F652A70/
//  6F6517B0" - the write-side loop RecordReserialize_6F652F20/6F652AC0
//  below call.  `this` = destStore, `edx` = arrayPtr, one stack arg
//  (size, always 0x10 at both call sites, forwarded to nothing since
//  the count comes from `*arrayPtr` instead), retn 4.
void* __fastcall WriteRecordArray78_6F652D20(void* destStore, void* arrayPtr, unsigned int /*size*/)
{
    unsigned char* arr = (unsigned char*)arrayPtr;
    unsigned char count = arr[0];
    ((CDataStoreScratch*)destStore)->WriteByte(count);

    if (count > 0)
    {
        char* entry = (char*)arrayPtr + 4;
        for (unsigned int i = 0; i < count; i++)
        {
            WriteRecordSlot_6F652A70(destStore, entry);
            entry += 0x78;
        }
    }
    return destStore;
}

void* __fastcall WriteRecordArray0C_6F651E20(void* destStore, void* arrayPtr, unsigned int /*size*/)
{
    unsigned char* arr = (unsigned char*)arrayPtr;
    unsigned char count = arr[0];
    ((CDataStoreScratch*)destStore)->WriteByte(count);

    if (count > 0)
    {
        char* entry = (char*)arrayPtr + 4;
        for (unsigned int i = 0; i < count; i++)
        {
            WriteRecordSlot_6F6517B0(destStore, entry);
            entry += 0x0C;
        }
    }
    return destStore;
}

//  sub_6F54CC10 - the shared "publish one re-serialized record" tail every
//  handler below ends in: if the record carries a non-null field (a
//  vtable slot 0xA call filling three out-params this session did not
//  chase - a foreign record class's own vtable, not one of this repo's
//  known ones), forward it into SubmitDecodedTurnPayload; either way
//  return whatever that returned.  `this` = self, 3 stack args (id,
//  record, zero), retn 0Ch.
typedef void (__thiscall *RecordFieldFetchFn)(void* record, unsigned int* outDest,
                                               unsigned int* outSize, unsigned int* outSize2);

//  0x6F54CAD0 - defined further below in this same file; forward-declared
//  here since SubmitReplayRecordViaCC10_6F54CC10 (also this batch) is its
//  one caller from above that point in the file.
int __fastcall SubmitDecodedTurnPayload_6F54CAD0(
    SNetSessionInfo* self, void* /*unused_edx*/,
    int a1, void* a2, int a3, int a4, int a5);

int __fastcall SubmitReplayRecordViaCC10_6F54CC10(
    SNetSessionInfo* self, void* /*unused_edx*/,
    unsigned char id, CDataStore* record, int zero)
{
    unsigned int outSize2 = 0;   // var_4
    unsigned int outSize  = 0;   // var_8
    unsigned int outDest  = 0;   // var_C

    if (record != 0)
    {
        RecordFieldFetchFn fn = (RecordFieldFetchFn)(*(void***)record)[0x28 / 4];
        fn(record, &outDest, &outSize, &outSize2);
        if (outSize == 0)
            outDest = 0;
    }
    else
    {
        outDest = 0;
    }

    return SubmitDecodedTurnPayload_6F54CAD0(self, 0, id, (void*)(size_t)outDest,
                                              (int)outSize, (int)outSize2, zero);
}

//  The 14 handler-specific record re-serializers (`ecx` = the fresh local
//  `CDataStore`-shaped store being built, `edx` = the just-decoded source
//  record).  Ten of these are this batch's own addresses; each builds
//  through the already-reconstructed CDataStoreScratch writers (cast
//  through the same CDataStore/CDataStoreScratch dual view this file's own
//  header already uses) plus the field/array helpers just above.  All
//  fourteen retn bare.
void __fastcall RecordReserialize_6F652510(void* destStoreVoid, void* srcRecord)
{
    CDataStoreScratch* dst = (CDataStoreScratch*)destStoreVoid;
    unsigned char* rec = (unsigned char*)srcRecord;
    dst->WriteDword(*(unsigned int*)(rec + 0));
    dst->WriteByte(rec[4]);
    SerializeLenPrefixedField_6F652270(dst, rec + 5);
    SerializeThreeStringsAndTwoDwords_6F652160(dst, rec + 0x20);
    dst->WriteDword(*(unsigned int*)(rec + 0xD8));
}

void __fastcall RecordReserialize_6F6527B0(void* destStoreVoid, void* srcRecord)
{
    CDataStoreScratch* dst = (CDataStoreScratch*)destStoreVoid;
    unsigned short len = *(unsigned short*)srcRecord;
    dst->WriteWord(len);
    dst->TailCallWriteRaw((char*)srcRecord + 2, len);
}

void __fastcall RecordReserialize_6F652750(void* destStoreVoid, void* srcRecord)
{
    CDataStoreScratch* dst = (CDataStoreScratch*)destStoreVoid;
    unsigned char* rec = (unsigned char*)srcRecord;
    dst->WriteByte(rec[0]);
    SerializeLenPrefixedField_6F652270(dst, rec + 1);
    dst->WriteDword(*(unsigned int*)(rec + 0x1C));
}

void __fastcall RecordReserialize_6F651570(void* destStoreVoid, void* srcRecord)
{
    CDataStoreScratch* dst = (CDataStoreScratch*)destStoreVoid;
    unsigned char* rec = (unsigned char*)srcRecord;
    dst->WriteDword(*(unsigned int*)(rec + 0));
    dst->WriteByte(rec[4]);
    dst->WriteDword(*(unsigned int*)(rec + 8));
    dst->WriteDword(*(unsigned int*)(rec + 0xC));
}

void __fastcall RecordReserialize_6F652840(void* destStoreVoid, void* srcRecord)
{
    CDataStoreScratch* dst = (CDataStoreScratch*)destStoreVoid;
    unsigned char* rec = (unsigned char*)srcRecord;
    unsigned char len = rec[0];
    dst->WriteByte(len);
    dst->TailCallWriteRaw(rec + 1, len);
}

void __fastcall RecordReserialize_6F652880(void* destStoreVoid, void* srcRecord)
{
    CDataStoreScratch* dst = (CDataStoreScratch*)destStoreVoid;
    unsigned char* rec = (unsigned char*)srcRecord;

    dst->WriteDword(*(unsigned int*)(rec + 0));
    unsigned char len = rec[4];
    dst->WriteByte(len);
    dst->TailCallWriteRaw(rec + 5, len);

    unsigned char count = rec[0x6C];
    dst->WriteByte(count);

    unsigned char* arr = *(unsigned char**)(rec + 0x70);
    for (unsigned int i = 0; i < count; i++)
        dst->WriteByte(arr[i]);
}

void __fastcall RecordReserialize_6F6516D0(void* destStoreVoid, void* srcRecord)
{
    CDataStoreScratch* dst = (CDataStoreScratch*)destStoreVoid;
    unsigned char* rec = (unsigned char*)srcRecord;
    dst->WriteByte(rec[0]);
    dst->WriteByte(rec[1]);
}

void __fastcall RecordReserialize_6F6527F0(void* destStoreVoid, void* srcRecord)
{
    CDataStoreScratch* dst = (CDataStoreScratch*)destStoreVoid;
    unsigned char* rec = (unsigned char*)srcRecord;
    dst->WriteByte(rec[0]);
    unsigned short len = *(unsigned short*)(rec + 1);
    dst->WriteWord(len);
    dst->TailCallWriteRaw(rec + 3, len);
}

void __fastcall RecordReserialize_6F652F20(void* destStoreVoid, void* srcRecord)
{
    CDataStoreScratch* dst = (CDataStoreScratch*)destStoreVoid;
    unsigned char* rec = (unsigned char*)srcRecord;
    dst->WriteDword(*(unsigned int*)(rec + 0));
    dst->WriteDword(*(unsigned int*)(rec + 4));
    WriteRecordArray78_6F652D20(destStoreVoid, rec + 8, 0x10);
}

void __fastcall RecordReserialize_6F652AC0(void* destStoreVoid, void* srcRecord)
{
    CDataStoreScratch* dst = (CDataStoreScratch*)destStoreVoid;
    unsigned char* rec = (unsigned char*)srcRecord;
    dst->WriteDword(*(unsigned int*)(rec + 0));
    WriteRecordArray0C_6F651E20(destStoreVoid, rec + 4, 0x10);
}

//  0x6F651610/1630/1650/1770 are NOT thunked here - each is already
//  reconstructed in netmsgio_651.cpp (declared above) and this macro
//  calls those directly, cast through the same
//  CDataStore/CDataStoreScratch dual view netgameresultblob.cpp's own
//  `CDataStoreScratch* ds = (CDataStoreScratch*)&stream;` already
//  establishes for this exact write-through-the-read-side-object shape.
static inline void RecordReserialize_6F651610(void* destStore, void* srcRecord)
{ NetMsgWriteDword_6F651610((CDataStoreScratch*)destStore, (const unsigned int*)srcRecord); }
static inline void RecordReserialize_6F651630(void* destStore, void* srcRecord)
{ NetMsgWriteDword_6F651630((CDataStoreScratch*)destStore, (const unsigned int*)srcRecord); }
static inline void RecordReserialize_6F651650(void* destStore, void* srcRecord)
{ NetMsgWriteDword_6F651650((CDataStoreScratch*)destStore, (const unsigned int*)srcRecord); }
static inline void RecordReserialize_6F651770(void* destStore, void* srcRecord)
{ NetMsgWriteDwordPair_6F651770((CDataStoreScratch*)destStore, (const unsigned int*)srcRecord); }

//  One handler, spelled out once and reused by macro for the other
//  thirteen - all fourteen share this exact shape, differing only in
//  which re-serializer they call.  A local `CDataStore` defined in a
//  different translation unit destructs through a real, out-of-line call
//  here rather than the inlined vtable-restamp-and-conditional-handback
//  the shipped code (which could see CDataStore's own definition) folds
//  in directly - the same accepted gap BuildGameResultBlob's own note
//  documents (netgameresultblob.cpp).
#define REPLAY_ECHO_HANDLER(fnName, reserializeFn)                             \
    int __fastcall fnName(SNetSessionInfo* self, void* /*unused_edx*/,         \
                           unsigned char id, void* record, int zero)           \
    {                                                                           \
        unsigned int capacity;                                                  \
        void* payload = PreparePayloadBuffer_6F650D40(0, &capacity);            \
                                                                                  \
        CDataStore localStore;                                                  \
        localStore.m_field4   = payload;                                        \
        localStore.m_field8   = 0;                                              \
        localStore.m_field0xC = (int)capacity;                                  \
        localStore.m_field10  = 0;                                              \
        localStore.m_readPos  = -1;                                             \
                                                                                  \
        reserializeFn(&localStore, record);                                     \
                                                                                  \
        return SubmitReplayRecordViaCC10_6F54CC10(self, 0, id, &localStore, zero); \
    }

REPLAY_ECHO_HANDLER(ReplayEcho_CameraBookmark_6F54DC60, RecordReserialize_6F652510)
REPLAY_ECHO_HANDLER(ReplayEcho_Handler_6F54DD30,        RecordReserialize_6F6527B0)
REPLAY_ECHO_HANDLER(ReplayEcho_Handler_6F54DE00,        RecordReserialize_6F652750)
REPLAY_ECHO_HANDLER(ReplayEcho_Handler_6F54DB90,        RecordReserialize_6F651570)
REPLAY_ECHO_HANDLER(ReplayEcho_Handler_6F54DED0,        RecordReserialize_6F651610)
REPLAY_ECHO_HANDLER(ReplayEcho_Handler_6F54DFA0,        RecordReserialize_6F651630)
REPLAY_ECHO_HANDLER(ReplayEcho_Handler_6F54E070,        RecordReserialize_6F651650)
REPLAY_ECHO_HANDLER(ReplayEcho_Handler_6F54E140,        RecordReserialize_6F652840)
REPLAY_ECHO_HANDLER(ReplayEcho_Handler_6F54E210,        RecordReserialize_6F652880)
REPLAY_ECHO_HANDLER(ReplayEcho_Handler_6F54E2E0,        RecordReserialize_6F6516D0)
REPLAY_ECHO_HANDLER(ReplayEcho_Handler_6F54E3B0,        RecordReserialize_6F6527F0)
REPLAY_ECHO_HANDLER(ReplayEcho_Handler_6F54E480,        RecordReserialize_6F651770)
REPLAY_ECHO_HANDLER(ReplayEcho_Handler_6F54E550,        RecordReserialize_6F652F20)
REPLAY_ECHO_HANDLER(ReplayEcho_Handler_6F54E620,        RecordReserialize_6F652AC0)
#undef REPLAY_ECHO_HANDLER

//  Per-record-type local-record builders (dispatcher-level - these decode
//  bytes straight off the wire into the record each handler above then
//  re-serializes) - `this` = self->m_replayStream, `edx` = &localRecord
//  (the destination the caller wants filled in - every one of these nine
//  reads it; see this file's own header note on the previous session's
//  "edx unused" defect), each a thin wrapper over CDataStore's own
//  ReadByte/ReadDword plus, for three of them, one further out-of-scope
//  helper.  retn bare.
CDataStore* __fastcall RecordCtor_6F6515B0(CDataStore* store, void* record)
{
    unsigned char* rec = (unsigned char*)record;
    store->ReadDword((unsigned int*)(rec + 0));
    store->ReadByte(rec + 4);
    store->ReadDword((unsigned int*)(rec + 8));
    store->ReadDword((unsigned int*)(rec + 0xC));
    return store;
}

CDataStore* __fastcall RecordCtor_6F651620(CDataStore* store, void* record)
{
    store->ReadDword((unsigned int*)record);
    return store;
}

CDataStore* __fastcall RecordCtor_6F651640(CDataStore* store, void* record)
{
    //  Byte-identical to RecordCtor_6F651620 above at a separate address -
    //  ordinary for this image, which was linked without /OPT:ICF
    //  (docs/msvc-vc8-idioms.md, "agentdefaults.cpp").
    store->ReadDword((unsigned int*)record);
    return store;
}

CDataStore* __fastcall RecordCtor_6F651660(CDataStore* store, void* record)
{
    //  A third /OPT:ICF-less duplicate of the same single-dword read.
    store->ReadDword((unsigned int*)record);
    return store;
}

//  0x6F651C60 - a length-prefixed field read: a word length via ReadWord;
//  if it comes back `<= 0x100`, that many raw bytes follow via ReadRaw and
//  the length itself is stored at `dest`; otherwise (EOF - ReadWord left
//  the length at its own `size+1` sentinel unread) bump the store's own
//  read-position counter instead.  Either way, if the store finished
//  short (m_readPos > m_field10) the length written at `dest` is zeroed
//  again - the same trailing "did this read actually land" guard every
//  sibling in this family (sub_6F651CF0 below) repeats.  `this` = store,
//  `edx` = dest, retn bare.
CDataStore* __fastcall ReadClampedField_6F651C60(CDataStore* store, void* dest)
{
    unsigned short len = (unsigned short)0x101;   // arg_size(0x100) + 1 sentinel
    store->ReadWord(&len);
    if (len <= 0x100)
    {
        store->ReadRaw((char*)dest + 2, len);
        *(unsigned short*)dest = len;
    }
    else
    {
        store->m_readPos = store->m_field10 + 1;
    }

    if ((unsigned int)store->m_readPos > (unsigned int)store->m_field10)
        *(unsigned short*)dest = 0;

    return store;
}

//  0x6F651CF0 - the byte-length twin of ReadClampedField_6F651C60 above
//  (clamp 0x60 instead of 0x100), reached from sub_6F652AA0 below.  `this`
//  = store, `edx` = dest, retn bare.  Already reconstructed under the
//  original session's own name, TurnsSyncFieldWrite_6F651CF0
//  (netevent_dispatch.cpp) - misleading (every call it actually makes,
//  CDataStore::ReadByte/ReadRawAlias, is a *read*, not a write), but a
//  second, independent redefinition under a better name is exactly the
//  "one shipped address, two names" defect class thunk_inventory.py's own
//  docs/notes/thunk-removal-pass.md flags, so this batch declares it
//  `extern` instead of redefining it.
extern CDataStore* __fastcall TurnsSyncFieldWrite_6F651CF0(void* store, void* dest);

//  0x6F651DC0 - "read `size` raw bytes one CDataStore::ReadByte at a time,
//  after first probing one more byte to tell a short/EOF read from a full
//  one" - the byte-array-of-N-flag-bytes twin of ReadFixedField16 below,
//  reached from sub_6F652AA0's own tail.  `this` = store, `edx` = dest,
//  one stack arg (size), retn 4.
CDataStore* __fastcall ReadRawBytesLoop_6F651DC0(CDataStore* store, void* dest, unsigned int size)
{
    unsigned char sz = (unsigned char)size;
    unsigned char tmp = (unsigned char)(sz + 1);
    store->ReadByte(&tmp);
    if (tmp <= sz)
    {
        store->m_readPos = store->m_field10 + 1;
        return store;
    }

    unsigned int count = tmp;
    *(unsigned int*)dest = count;
    for (unsigned int i = 0; i < count; i++)
        store->ReadByte((unsigned char*)dest + 4 + i);

    return store;
}

//  0x6F652AA0 - one 0x78-stride array entry ReadFixedField16_6F652D60's
//  own loop reads through: a clamped-length field (ReadClampedField60h_
//  6F651CF0) followed by a fixed 16-byte raw run at the entry's own
//  +0x64 (ReadRawBytesLoop_6F651DC0).  `this` = store, `edx` = entry,
//  retn bare.
void* __fastcall ReadRecordSlot_6F652AA0(void* store, void* entry)
{
    CDataStore* s = (CDataStore*)store;
    TurnsSyncFieldWrite_6F651CF0(s, entry);
    ReadRawBytesLoop_6F651DC0(s, (char*)entry + 0x64, 0x10);
    return store;
}

//  0x6F6517E0 - the 0x0C-stride sibling entry reader ReadFixedField16_
//  6F651E60's own loop reads through instead: a byte then two dwords, all
//  through already-reconstructed CDataStore readers.  `this` = store,
//  `edx` = dest, retn bare.
CDataStore* __fastcall ReadRecordSlot_6F6517E0(CDataStore* store, void* dest)
{
    store->ReadByte((unsigned char*)dest);
    store->ReadDword((unsigned int*)((char*)dest + 4));
    store->ReadDword((unsigned int*)((char*)dest + 8));
    return store;
}

CDataStore* __fastcall RecordCtor_6F652820(CDataStore* store, void* record)
{
    unsigned char* rec = (unsigned char*)record;
    store->ReadByte(rec + 0);
    ReadClampedField_6F651C60(store, rec + 1);
    return store;
}

//  0x6F652D60 / 0x6F651E60 - "read one more byte to tell a short/EOF read
//  from a real element count, then either bump the read-position counter
//  (short read) or read that many 0x78-/0x0C-stride array entries through
//  ReadRecordSlot_6F652AA0/6F6517E0" - the exact same clamp-probe shape
//  ReadClampedField_6F651C60 and ReadRawBytesLoop_6F651DC0 above already
//  use, one level up (an array of records rather than an array of raw
//  bytes).  `this` = store, `edx` = dest, one stack arg (size, always
//  0x10 at both call sites), retn 4.
CDataStore* __fastcall ReadFixedField16_6F652D60(CDataStore* store, void* dest, unsigned int size)
{
    unsigned char sz = (unsigned char)size;
    unsigned char tmp = (unsigned char)(sz + 1);
    store->ReadByte(&tmp);
    if (tmp <= sz)
    {
        store->m_readPos = store->m_field10 + 1;
        return store;
    }

    unsigned int count = tmp;
    *(unsigned int*)dest = count;
    if (count > 0)
    {
        char* entry = (char*)dest + 4;
        for (unsigned int i = 0; i < count; i++)
        {
            ReadRecordSlot_6F652AA0(store, entry);
            entry += 0x78;
        }
    }
    return store;
}

CDataStore* __fastcall ReadFixedField16_6F651E60(CDataStore* store, void* dest, unsigned int size)
{
    unsigned char sz = (unsigned char)size;
    unsigned char tmp = (unsigned char)(sz + 1);
    store->ReadByte(&tmp);
    if (tmp <= sz)
    {
        store->m_readPos = store->m_field10 + 1;
        return store;
    }

    unsigned int count = tmp;
    *(unsigned int*)dest = count;
    if (count > 0)
    {
        char* entry = (char*)dest + 4;
        for (unsigned int i = 0; i < count; i++)
        {
            ReadRecordSlot_6F6517E0(store, entry);
            entry += 0x0C;
        }
    }
    return store;
}

CDataStore* __fastcall RecordCtor_6F652F50(CDataStore* store, void* record)
{
    unsigned char* rec = (unsigned char*)record;
    store->ReadDword((unsigned int*)(rec + 0));
    store->ReadDword((unsigned int*)(rec + 4));
    ReadFixedField16_6F652D60(store, rec + 8, 0x10);
    return store;
}

CDataStore* __fastcall RecordCtor_6F652AE0(CDataStore* store, void* record)
{
    unsigned char* rec = (unsigned char*)record;
    store->ReadDword((unsigned int*)(rec + 0));
    ReadFixedField16_6F651E60(store, rec + 4, 0x10);
    return store;
}

//----------------------------------------------------------------------------
//  sub_6F548830 - a small helper the case-48-shaped record (table2 case
//  48's own EB00 twin) calls before its own RecordCtor_6F652F50: zero-
//  initialize a 16-entry, 0x78-byte-stride array (`buf+0xC .. buf+0x78B`),
//  each entry's own +0x64 word set to the same 0x10 capacity constant
//  ReadFixedField16 above reads with, and the array's own declared count
//  (`buf+8`) set to 0x10.  `this` only (a local buffer), retn bare.
//----------------------------------------------------------------------------
void __fastcall PreInitRecord_6F548830(void* buf)
{
    *(unsigned int*)((char*)buf + 8) = 0x10;

    char* entry = (char*)buf + 0xC;
    int i = 15;
    do
    {
        *entry = 0;
        *(unsigned int*)(entry + 0x64) = 0x10;
        entry += 0x78;
    }
    while (--i >= 0);
}

//----------------------------------------------------------------------------
//  0x6F54CAD0 - SubmitDecodedTurnPayload.  The "submit decoded turn
//  payload" tail table2 cases 30/31's own EB00 twin (byte values 0x10/
//  0x11) reaches once a raw payload has been read off the stream, and the
//  shared publish tail (SubmitReplayRecordViaCC10_6F54CC10) also reaches
//  once a re-serialized replay record has a non-null field.  `this` =
//  self, 5 stack args (id/flag byte, dest ptr, size, a second size, a
//  zero that both known callers always pass as literal 0 - the trailing
//  `bool` this returns is `arg_10 != 0`, i.e. always false on every call
//  site this session found), retn 0x14.
//----------------------------------------------------------------------------

//  sub_6F54B810 / sub_6F548FF0 / sub_6F657030 - the "bad event, log and
//  release" fallback, the cache-touch helper's own tail, and the
//  type-specific notify tail's own forwarding call.  Each is a genuine
//  entry point into the ~40-strong CNetEvent/event-queue pool family
//  netevent_dispatch.cpp's own header already defers - out of scope for
//  this batch, but their own dumps were pulled (asm/sub_6F548FF0_*.md
//  etc.) so each retn below is confirmed rather than guessed.
//
//  AllocEventSlot_6F549B80 itself is real now (netdataeventqueuealloc.cpp,
//  declared in netdataeventqueue.h) - a genuine reconstruction of the
//  event-slot allocator, not deferred with the other three.
NAKED_THUNK(void __fastcall LogBadEventSlot_6F54B810(SNetSessionInfo* self, void* pendingSlot), 0x6F54B810)
NAKED_THUNK(void __fastcall CacheTouch_6F548FF0(void* handle, int flag, int zero), 0x6F548FF0)
NAKED_THUNK(void __fastcall NotifyEventQueue_6F657030(void* queueSlot, void* self, void* size), 0x6F657030)

//  0x6F54C490 - build an event slot (AllocEventSlot_6F549B80) from the
//  session's own +0x2290 queue and, if `id`/`size`/`size2` all pass their
//  own 0xFF/0xFFFFF range checks, fill it in and return it; otherwise log
//  the bad event and return null.  `this` = self, 5 stack args (id, dest,
//  size, size2, zero), retn 14h.
void* __fastcall LookupOrEnqueueEvent_6F54C490(
    SNetSessionInfo* self, void* /*unused_edx*/,
    unsigned char id, void* dest, int size, int size2, int zero)
{
    void* slot = AllocEventSlot_6F549B80((char*)self + 0x2290);

    if (id < 0xFF && (unsigned int)size < 0xFFFFF && (unsigned int)size2 < 0xFFFFF)
    {
        *((unsigned char*)slot + 0x14) = id;
        *(void**)((char*)slot + 8)     = dest;
        *(int*)((char*)slot + 0xC)     = size;
        *(int*)((char*)slot + 0x10)    = size2;
        *((unsigned char*)slot + 0x15) = (unsigned char)zero;
        return slot;
    }

    LogBadEventSlot_6F54B810(self, slot);
    return 0;
}

//  0x6F5491B0 - `ecx`/`edx` genuinely unread by the shipped body (the
//  caller's own `self+0xF38` "this" is dead weight); the one stack
//  argument (`handle`) is all it forwards, plus two fixed constants, into
//  CacheTouch_6F548FF0.  One stack arg, retn 4.
void __fastcall TouchEventCache_6F5491B0(void* /*obj, unused*/, void* /*unused_edx*/, void* handle)
{
    CacheTouch_6F548FF0(handle, 2, 0);
}

//  sub_6F6516C0 - already reconstructed as NetMsgReadWord_6F6516C0
//  (netmsgio_651.cpp) - redeclared here for extern linkage rather than
//  re-thunked under a second name.  netcommand_dispatch.cpp used to carry
//  a second, independent body for this same address under a third name
//  (CNetData_ReadRecordCount) - the "one shipped address, two names" case
//  this comment originally flagged as out of scope; a later session
//  collapsed it onto this reconstruction after gen_rename_from_funcmap.py's
//  conflict check caught it.
extern CDataStore* __fastcall NetMsgReadWord_6F6516C0(CDataStore* self, unsigned short* dst);

//  0x6F650DB0 - if `self` is non-null, fetch this thread's own +0xE
//  thread-local slot (GetThreadLocalSlot, gamecontext.h) and, if set,
//  forward through it; otherwise (both null-`self` and empty-slot cases)
//  release `self` through SMemFree, tagged with this same module's own
//  filename constant every other CDataStore-family teardown in this file
//  uses (cdatastorecache.cpp, cdatastorescratch.cpp).  `this` = self,
//  `edx` = size (only read in the forwarding path), retn bare.
void __fastcall NotifyTurnPayload_6F650DB0(void* self, void* size)
{
    if (self != 0)
    {
        void* slot = GetThreadLocalSlot(0xE);
        if (slot != 0)
        {
            NotifyEventQueue_6F657030(slot, self, size);
            return;
        }
    }

    SMemFree(self, "e:\\drive1\\temp\\buildwar3x\\engine\\source\\netgame.cpp", 0x108, 0);
}

//  0x6F686280 - the same null-guarded SMemFree tail as
//  NotifyTurnPayload_6F650DB0 above, minus the thread-local-slot
//  forwarding branch: `this` (here `self`, ecx) null skips straight to
//  the return, otherwise the same fixed string goes to SMemFree.  `edx`
//  (`size`) is dead - never read.  Retn bare.
void __fastcall NotifyTurnPayloadAlt_6F686280(void* self, void* /*size, unused*/)
{
    if (self != 0)
        SMemFree(self, "e:\\drive1\\temp\\buildwar3x\\engine\\source\\netgame.cpp", 0x108, 0);
}

int __fastcall SubmitDecodedTurnPayload_6F54CAD0(
    SNetSessionInfo* self, void* /*unused_edx*/,
    int a1, void* a2, int a3, int a4, int a5)
{
    unsigned char id = (unsigned char)a1;
    void* dest = a2;
    int size = a3;
    int size2 = a4;
    int zero = a5;

    void* handle = LookupOrEnqueueEvent_6F54C490(self, 0, id, dest, size, size2, zero);
    if (handle != 0)
    {
        TouchEventCache_6F5491B0((char*)self + 0xF38, 0, handle);

        if (id == 0x1E || id == 0x1F)
        {
            //  Wrap `dest`/`size` (a2/a3) in a throwaway, *unowned*
            //  (m_field0xC == -1, so its own conditional teardown never
            //  fires) CDataStore view and pull one length word off it
            //  through the already-reconstructed NetMsgReadWord_6F6516C0
            //  (netmsgio_651.cpp).  On success, tally the word into the
            //  session's own +0x225C running counter (not otherwise
            //  named - the only reader/writer either dump reaches).
            CDataStore localStore;
            localStore.m_field4   = dest;
            localStore.m_field8   = 0;
            localStore.m_field0xC = -1;
            localStore.m_field10  = size;
            localStore.m_readPos  = 0;

            unsigned short lenOut = 0;
            NetMsgReadWord_6F6516C0(&localStore, &lenOut);

            if ((unsigned int)localStore.m_readPos <= (unsigned int)localStore.m_field10)
            {
                *(unsigned int*)((char*)self + 0x225C) += lenOut;
            }
            //  ~CDataStore runs here - see the header note on the same
            //  cross-TU-visibility gap the 14 handlers above accept.
        }
    }
    else
    {
        nullsub_1("CNetData::EnqueueEvent: bad event %u[%u]\n", (unsigned int)id, size2);

        if (id < 0x40)
            NotifyTurnPayload_6F650DB0(dest, (void*)(size_t)size2);
        else if (id < 0xFF)
            NotifyTurnPayloadAlt_6F686280(dest, (void*)(size_t)size2);
    }

    //  Genuinely `a5 != 0`, not `handle != 0` - the dump's own tail
    //  compares arg_10 (this function's fifth stack argument, `zero`
    //  above), which every known call site always passes as literal 0,
    //  so this always returns false in practice.  Kept as the real
    //  comparison rather than simplified to `return false;` - the next
    //  caller found passing a1==0 for `a5` would make this observable.
    return zero != 0;
}
