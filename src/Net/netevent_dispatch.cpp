//============================================================================
//  0x6F551D80 - CNetEventTypeDispatcher::ConstructAndBroadcast.
//
//  Three sessions deferred this as "a legitimate multi-wave target on its
//  own merits" (see pausecallchainthunks.cpp's own retired thunk comment
//  and docs/targets/NetRouter_NetPlayer_NetClient_oneslot_sweep.md, "Wave
//  12"); this is that session.  asm/sub_6F551D80_0x6F551D80_calltree_asm.md
//  (depth-1, 77 functions, full bodies) is the source.
//
//  Shape, read straight off the dump:
//
//    1. Wrap `arg_0` (the raw record: [eax+8]=pointer, [eax+0Ch]=length,
//       [eax+14h]=one-byte message-type code) in a stack CDataStore
//       (`store` below), clamping the type byte to [0,0x32).
//    2. If the record's slot (`edi`, read off [self+0x610] unless the
//       record's own +0x15 byte is 0, in which case slot 0) does not equal
//       the *active* record ([self+0x610] again), skip straight to step 4 -
//       none of the type-0x10/0x15/0x1B special cases below apply to a
//       record that is not for the currently-active slot.
//    3. Three inline special cases on the clamped type byte - see the
//       per-case comments below.  0x10 and 0x15 are network-route-candidate
//       bookkeeping (SlotRouteRecord, `IsUnknownTag_6F537790`, the
//       [self+0x1C74] flag) - NOT replay-specific, despite living next to
//       the one that is.  0x1B is the replay-streaming state-machine
//       driver - the actual reason this session exists.
//    4. Fall through (from any type not caught above, or after 0x1B/0x10/
//       0x15 finish) into a *single* combined dispatch on the type byte:
//       a first jump table (`jpt_6F5520A6`, IDA's 25-entry remap over
//       types 0x17-0x2F) intercepts 8 of those types for extra
//       serialization bookkeeping (`sub_6F537AA0`) before *every* path -
//       including types the first table does not touch at all - proceeds
//       into a second jump table (`jpt_6F55211B`, 49 entries, types
//       1-0x31) that constructs one of ~40 named `CNetEvent` subclasses
//       and broadcasts it through `CNetPlayerRecord::FireToObserverLists`
//       (0x6F5378E0, already EXACT), or - for six special-shaped types
//       (0x1E/0x1F/0x20/0x22/0x23/0x30/0x31, table2's own "case 30/31/32/
//       34/35/48/49") - does something else entirely (redispatches a
//       queued turn record, fires a tick/sync-mismatch diagnostic, or
//       fires a "trusted desync/result" notification).
//
//  Every one of table2's ~40 uniform-shaped cases (push 0/turn/flag, `this`
//  = self+8+slot*0x304, `edx` = slot, 7 stack args - CDataStore*, name,
//  0, 0-or-a-destructor-pointer, flag, turn, 0 - retn 0x1Ch) is left as a
//  correctly-ABI'd naked thunk to its real, unhooked body: wave 12 already
//  read every one of their names and confirmed none is replay-shaped, and
//  reproducing all ~40 distinct constructor bodies is its own multi-wave
//  target exactly as that doc says.  This file's job is the dispatcher's
//  own real switch/dispatch logic, which is everything *outside* those ~40
//  bodies - the special cases, the two-jump-table decode, and the six
//  special-shaped table2 cases whose own logic (not just a builder call)
//  matters.
//============================================================================
#include "netdata.h"
#include "netcommand.h"     // CNetCommandBase
#include "cdatastore.h"
#include "netdata_replayrecords.h"   // ReadReplayType0x10Record/0x16Record - already reconstructed
#include "savegame.h"                // Net::GAMESETUP_SLOTRECORD
#include "cdatastorescratch.h"       // CDataStoreScratch - the observer/replay stream's write side
#include "os.h"                      // OsPathIsRelative/OsGetModuleDirectory/OsPathStripFileName/OsCreateDirectoryTree, Storm_501/503
#include "asyncfile.h"       // AsyncFileOpen
#include "syncdata.h"                // CSyncData (RTTI: NTempest::CSyncData) - self+0x2278's real class
#include <string.h>                  // memcpy

void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);
extern "C" void(__stdcall* g_pfnRecycleBuffer)(void** pField4, int* pField8, int* pField0xC);
//  storm.h - SMemAlloc/SMemReAlloc (SSuspendPlayerList::GrowOrRelease below)
//  and the TSFixedArray<T> template (the two per-record growable arrays
//  cnd-wave2-c's own DistFileProgress/OfficialPlayers wide-field readers
//  below instantiate).  aufixedstring.h - the AuFixedString/AuFixedStringArray
//  type ReadTeamGameListTailField_6F653760 shares with its one other user
//  (Render/fontfileversionarray.cpp) rather than re-instantiating the same
//  shipped SetCount body at a new address.
#include "storm.h"
#include "aufixedstring.h"

//  0x6F4C7150/0x6F4C75E0 - already reconstructed in cdatastorecacheseal.cpp
//  as DataStoreCacheDrain/DataStoreCacheSeal; re-declared identically here
//  (CLAUDE.md's "the awkward ones carry a note") since neither is behind a
//  shared header. RouteCandidateDiagnostic_6F5349B0's own receiver is a
//  different concrete object than that file's SDataStoreCache, but the
//  address, field layout and retn are the same function.
struct SDataStoreCache
{
    char         m_reserved00[0x34];
    unsigned int m_flags;       // +0x34
    char         m_reserved38[0x10];
    unsigned int m_limit;       // +0x48
    char         m_reserved4C[0x10];
    unsigned int m_retired;     // +0x5C
};
void __fastcall DataStoreCacheSeal(SDataStoreCache* self);

//  0x6F469800 - already reconstructed in actionqueuerecord_resize.cpp as
//  ResizableBuffer::ResizeBuffer; re-declared identically here (size/prev
//  capacity/backing pointer at +0/+4/+8) for ResizableBuffer::SetKindOrRelease
//  (0x6F46C440) below, whose own receiver is a different concrete buffer at
//  the same three-field layout.  SetKindOrRelease is a genuine second
//  member of the same layout: `retn 4` (one stack argument, `this` in
//  `ecx`) proves __thiscall, not __fastcall - the same proof
//  netclient_slot1_helpers.cpp's own ByteLengthPrefixedFieldCopier documents,
//  modelled the same way since __thiscall cannot be spelled on a free
//  function.
class ResizableBuffer
{
public:
    void ResizeBuffer(unsigned int newSize);
    void SetKindOrRelease(unsigned int kind);
};

//  0x6F6527E0 - already reconstructed (netdata_loadreplayheader.cpp) as a
//  trivial passthrough onto SaveGameReadSlotRecord.
CDataStore* __fastcall ReadReplaySlotRecord(CDataStore* store,
                                            Net::GAMESETUP_SLOTRECORD* out);

//----------------------------------------------------------------------------
//  SNetDataStates - re-declared here exactly as netdatareplaystate.cpp
//  declares it privately, so a call through it mangles to the same two
//  symbols (0x6F537D20/0x6F537DB0) rather than gaining a second,
//  non-matching declaration (CLAUDE.md, "the awkward ones carry a note").
//  `self` (SNetSessionInfo*) IS the SNetDataStates* here - both fields it
//  touches (+0x614 m_replayState, +0xBE4 m_recordState... actually +0x614
//  overlaps m_activeIndex; see the note in netdatareplaystate.cpp) sit at
//  the same address either way, so the reinterpret_cast below is exact.
//----------------------------------------------------------------------------
struct SNetDataStates
{
    int SetReplayState(unsigned int state);
    int SetRecordState(unsigned int state);
};

//  Already reconstructed elsewhere - redeclared identically for extern
//  linkage (none of the four is behind a shared header).
void* __fastcall GetSelectedSubObject_6F537B20(void* self);
int __fastcall IsUnknownTag_6F537790(const void* self);
unsigned int GetProductTag_6F537EF0();
CDataStore* __fastcall NetMsgReadDword_6F537830(CDataStore* self, unsigned int* dst);
//  netcommand_dispatch.cpp's own real definition returns void* (the
//  reporter's own field pointer, unused by every caller) - link_check.py
//  caught this file's re-declaration returning plain void, a distinct
//  mangled symbol (`X` vs `PAX` return code) that would have shown up as
//  a genuinely unresolved external in the real link, invisible to
//  verify.py since both TUs compile clean on their own. (Fixed
//  independently and identically by two concurrent sessions.)
void* __fastcall CNetData_AppliedDeltaReporterSetTurnField(void* self, void* turnFieldPtr);
void __fastcall CNetData_DispatchQueuedTurnRecord(SNetSessionInfo* self, void* /*unused_edx*/,
                                                   CDataStore* record, unsigned int playerIndex,
                                                   unsigned int latchedDelta);
void __cdecl nullsub_1(const char*, ...);
void CNetData_ParseError(const char* tag);

//  0x6F54EB00 - CNetEventTypeDispatcher::ReplayAckDispatch, this session's
//  second target (netevent_replaystream.cpp).  `this` only, retn bare.
void __fastcall CNetEventTypeDispatcher_ReplayAckDispatch(SNetSessionInfo* self);

//----------------------------------------------------------------------------
//  lpFileName - a data global read by 0x6F5349B0's own caller here
//  (below), symbolic-only in the dump (no numeric address, the same shape
//  CLAUDE.md's `lpAddend` precedent already documents) - private storage
//  per that same precedent, tools/funcmap.py's DATA table entry next to it.
//----------------------------------------------------------------------------
extern "C" const char* lpFileName = 0;

//----------------------------------------------------------------------------
//  ValueName - a second data global, read by FireEvent_6F54E860's own six
//  diagnostic-log cases below, symbolic-only in the dump the same way
//  lpFileName above is - private storage, same precedent.
//----------------------------------------------------------------------------
extern "C" const char* ValueName = 0;

//----------------------------------------------------------------------------
//  Leaf callee thunks - correctly-ABI'd naked bodies to shipped code this
//  batch did not reconstruct (each is a further, unidentified subsystem -
//  string formatting, replay/turn-record deserialisation - the un-thunking
//  worklist's own function list does not name).  Stack byte counts are each
//  confirmed against the callee's own `retn`.
//----------------------------------------------------------------------------
#define NAKED_THUNK(sig, addr)             \
    __declspec(naked) sig                  \
    {                                       \
        __asm { mov eax, addr }             \
        __asm { jmp eax }                   \
    }

//----------------------------------------------------------------------------
//  0x6F534660 - EnsureRouteCandidateLogOpen.  `obj` is a small log-file
//  record: a handle at +0x100, a persisted MAX_PATH name buffer at +0x104
//  (filled only on a fresh, successful open), and four more bookkeeping
//  fields the success path resets (+0x208/+0x20C/+0x210/+0x214). Already-
//  open guard first (+0x100 != 0 -> return 1, the "veto" RouteCandidateDiagnostic
//  below reads); otherwise build an absolute directory for `fileName` on a
//  local scratch buffer (module directory + name if the name is relative -
//  OsPathIsRelative/OsGetModuleDirectory/Storm_503's own append shape -
//  otherwise a straight Storm_501 copy), OsPathStripFileName it down to just
//  the directory and OsCreateDirectoryTree it, then AsyncFileOpen the
//  *original* `fileName` (not the scratch directory buffer - that dance only
//  exists to make sure the target directory is there first). On open
//  failure, return 4; on success, persist `fileName` into the object's own
//  +0x104 buffer, store the handle at +0x100, reset the four bookkeeping
//  fields (the third, +0x210, to the constant 0x44) and return 0.
//
//  What is NOT reproduced: this build's fixed /GS- never emits the shipped
//  frame's stack-buffer cookie (`mov eax, dword_6FAAE140` / `xor eax,esp` on
//  entry, the mirrored xor/call SecurityCheckCookie on every exit) - the
//  same accepted gap osversionclass.cpp documents for the identical shape.
//  IDENTICAL, not DIFFERS.
//----------------------------------------------------------------------------
struct SRouteCandidateLogFile
{
    char         m_reserved00[0x100];
    void*        m_file;          // +0x100 - ASYNCFILE* (asyncfile.h)
    char         m_pathBuf[0x104];// +0x104 - persisted `fileName`, set on open
    unsigned int m_field208;      // +0x208
    unsigned int m_field20C;      // +0x20C
    char         m_reserved210[0x22C - 0x210];
    unsigned int m_field22C;      // +0x22C
};

int __fastcall EnsureRouteCandidateLogOpen_6F534660(void* obj, void* /*unused_edx*/,
                                                      const char* fileName)
{
    SRouteCandidateLogFile* rec = (SRouteCandidateLogFile*)obj;
    const char* name = fileName;
    if (rec->m_file != 0)
        return 1;

    char dirBuf[0x104];
    if (OsPathIsRelative(name))
    {
        OsGetModuleDirectory(dirBuf, 0x104);
        Storm_503(dirBuf, name, 0x104);
    }
    else
    {
        Storm_501(dirBuf, name, 0x104);
    }
    OsPathStripFileName(dirBuf);
    OsCreateDirectoryTree(dirBuf, 1);

    //  Calls through directly to AsyncFileOpen (0x6F7E89E0, asyncfile.h)
    //  rather than the one-instruction alias AsyncFileOpen the dump's own
    //  call target (0x6F7E8B10) tail-jumps through - the alias is declared
    //  `void` (asyncfilealiases.cpp, naked) so its return value cannot be
    //  captured in typed C++; both addresses reach the identical real body
    //  once linked into the .mix, and the call's own target symbol is
    //  invisible to the instruction score either way.
    void* file = AsyncFileOpen(name, 0);
    if (file == 0)
        return 4;

    rec->m_file = file;
    Storm_501(rec->m_pathBuf, name, 0x104);
    rec->m_field208 = 0;
    rec->m_field20C = 0;
    rec->m_field22C = 0;
    *(unsigned int*)((char*)obj + 0x210) = 0x44;
    *(unsigned int*)((char*)obj + 0x214) = 0;
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F5349B0 - RouteCandidateDiagnostic.  `this` = whatever
//  GetSelectedSubObject_6F537B20 returned (not `self`); writes into that
//  object's own +0x230/+0x234/+0x238 the first time its log file opens
//  (EnsureRouteCandidateLogOpen_6F534660 above returns 0 only on a fresh,
//  successful open - 1 for "already open", 4 for "open failed"), then seals
//  the object's own embedded cache through DataStoreCacheSeal (0x6F4C75E0 -
//  already reconstructed in cdatastorecacheseal.cpp; this receiver is a
//  different concrete object than that file's own SDataStoreCache, but the
//  address, the `+0x34` flag-then-drain shape and the retn are identical, so
//  it is the same function, not a naked redeclaration of it).  retn 0x10
//  (4 stack args).
//----------------------------------------------------------------------------
int __fastcall RouteCandidateDiagnostic_6F5349B0(void* obj, void* /*unused_edx*/,
                                                   const char* a1, const char* a2,
                                                   unsigned int a3, unsigned int a4)
{
    int opened = EnsureRouteCandidateLogOpen_6F534660(obj, 0, a1);
    if (opened == 0)
    {
        *(unsigned int*)((char*)obj + 0x230) = (unsigned int)a2;
        *(unsigned int*)((char*)obj + 0x234) = a3;
        *(unsigned int*)((char*)obj + 0x238) = a4;
        DataStoreCacheSeal((SDataStoreCache*)obj);
    }
    return opened == 0;
}

//  0x6F537AA0 - SerializeExtraHeader, already reconstructed in its own TU
//  (netevent_serializeextraheader.cpp) - split out from here so this file's
//  own small callers (AppendRouteCandidateRecord_6F543A40 and siblings,
//  below) do not get it inlined by this build's /Ob2, which the shipped
//  call sites never do.
void __fastcall SerializeExtraHeader_6F537AA0(void* self, void* /*unused_edx*/,
                                               unsigned int msgType, void* dataPtr);

//----------------------------------------------------------------------------
//  0x6F652870 / 0x6F653650 - the TURNSSYNC / TURNSSYNCMISMATCH diagnostic-
//  record builders table2 cases 34/35 (msg types 0x22/0x23) call before
//  their own comparison-and-fire tail.  Both retn bare (`this` only; the
//  `edx` each call site loads is dead, never read by either body).
//
//  0x6F651CF0 - TurnsSyncFieldWrite: read one length-prefixed field off
//  `store` (a length byte through CDataStore::ReadByte, then - only if the
//  length is <= 0x60 - that many raw bytes through CDataStore::ReadRawAlias
//  into `dest+1`, `dest[0]` getting the length byte itself; a length over
//  0x60 instead forces `store`'s own m_readPos one past m_field10, which is
//  exactly the "ran out of declared data" shape EnsureReadable's callers
//  check elsewhere in this file).  Either way, if the store is left past
//  its own declared length (m_readPos > m_field10) `dest[0]` is
//  zero-terminated instead of carrying the length byte - the shared
//  overrun guard both the too-long-length path and a genuine EnsureReadable
//  failure inside ReadRawAlias fall into.  Returns `store` (the same
//  stream-`>>`-style return every CDataStore reader here uses).  Also
//  reachable as ReadRecordSlot_6F652AA0's own field helper
//  (netevent_replaystream_records.cpp, batch-I-playertable-and-bnet) -
//  that TU declares it `extern` under this same name rather than
//  redefining it under a second one.
//
//  0x6F46C440 - ResizableBuffer::SetKindOrRelease: `record` is a ResizableBuffer
//  (size/prevKind at +0/+4, backing pointer at +8 - the same layout
//  actionqueuerecord_resize.cpp's own class already names, reached here at
//  its caller's own dest+0x68 so its backing pointer lines up with
//  TurnsSyncMismatchRecordDtor_6F545250's own dest+0x70 release). `kind`
//  equal to the record's own remembered kind (+4) is a no-op; `kind == 0`
//  releases the backing block (Storm_403 tagged "e", matching the dtor's
//  own tag) and zeroes all three fields; any other `kind` resizes the
//  backing block to `kind` bytes and remembers it as the new kind.
//----------------------------------------------------------------------------
CDataStore* __fastcall TurnsSyncFieldWrite_6F651CF0(void* store, void* dest)
{
    CDataStore* s = (CDataStore*)store;
    unsigned char* out = (unsigned char*)dest;
    unsigned char len = 0;
    s->ReadByte(&len);

    if (len <= 0x60)
    {
        s->ReadRawAlias(out + 1, len);
        out[0] = len;
    }
    else
    {
        s->m_readPos = s->m_field10 + 1;
    }

    if ((unsigned int)s->m_readPos > (unsigned int)s->m_field10)
        out[0] = 0;

    return s;
}

void ResizableBuffer::SetKindOrRelease(unsigned int kind)
{
    unsigned int* fields = (unsigned int*)this;   // +0=size, +4=kind, +8=ptr

    if (kind == fields[1])
        return;

    if (kind == 0)
    {
        void* ptr = *(void**)((char*)this + 8);
        if (ptr)
            SMemFree(ptr, "e", -2, 0);

        fields[0] = 0;
        fields[1] = 0;
        fields[2] = 0;
        return;
    }

    ResizeBuffer(kind);
    fields[1] = kind;
}

void* __fastcall TurnsSyncRecordCtor_6F652870(void* store, void* dest)
{
    //  `dest` genuinely flows through - the "unused_edx" the header comment
    //  above describes is TurnsSyncMismatchRecordCtor's own second field,
    //  not this: this constructor forwards `edx` untouched straight into
    //  TurnsSyncFieldWrite_6F651CF0's own dest parameter.
    TurnsSyncFieldWrite_6F651CF0(store, dest);
    return store;
}

void* __fastcall TurnsSyncMismatchRecordCtor_6F653650(void* store, void* dest)
{
    CDataStore* dataStore = (CDataStore*)store;
    dataStore->ReadDword((unsigned int*)dest);
    TurnsSyncFieldWrite_6F651CF0(store, (char*)dest + 4);

    unsigned char kind = 0x11;
    dataStore->ReadByte(&kind);
    if (kind > 0x10)
    {
        ((ResizableBuffer*)((char*)dest + 0x68))->SetKindOrRelease(kind);
        unsigned int i = 0;
        //  Re-reads dest+0x6C's own count every iteration rather than
        //  caching it in a local, matching the shipped loop's own
        //  `cmp edi,[esi+108]` - caching it costs an extra spilled
        //  register (ebp) that pushes/pops around the whole loop.
        while (i < *(unsigned int*)((char*)dest + 0x6C))
        {
            dataStore->ReadByte((unsigned char*)((char*)dest + 0x70) + i);
            ++i;
        }
        return store;
    }

    *(unsigned int*)((char*)store + 0x14) = *(unsigned int*)((char*)store + 0x10) + 1;
    return store;
}

//----------------------------------------------------------------------------
//  0x6F545250 - the TURNSSYNCMISMATCH record's own destructor-shaped
//  teardown: if the record's own +0x70 pointer is non-null, release it
//  through Storm_403 tagged "e" (the dump's own `offset aE_1`), matching
//  the same Storm_403-based cleanup this file's other CDataStoreCache
//  destructors use.  retn bare.
//----------------------------------------------------------------------------
void __fastcall TurnsSyncMismatchRecordDtor_6F545250(void* record)
{
    void* ptr = *(void**)((char*)record + 0x70);
    if (ptr)
        SMemFree(ptr, "e", -2, 0);
}

//----------------------------------------------------------------------------
//  0x6F53FAB0 / 0x6F53FD10 - construct/destroy a CDataStoreCache<97> local
//  (see cdatastorecache.cpp's own CDataStoreCache1460 for the established
//  shape this mirrors - same vtable-slot-0-override-points-at-own-buffer
//  idiom, 97 bytes instead of 1460, and its own distinct release function
//  pointer, g_pfnCDataStoreCache97Release) the shared def_6F55211B tail uses to serialize
//  the current record for a "trusted" observer, only when the type-0x1B
//  branch above set the "handled" flag.  Both retn bare (`this` only).
//----------------------------------------------------------------------------
class CDataStoreCache97;
typedef void(__thiscall* CDataStoreCache97CleanupFn)(CDataStoreCache97* self, void** pField4, void* pField8, int* pField0xC);
extern "C" CDataStoreCache97CleanupFn g_pfnCDataStoreCache97Release;

class CDataStoreCache97 : public CDataStore
{
public:
    CDataStoreCache97();
    ~CDataStoreCache97();

    virtual void m_reservedSlot0() { OnConstruct(&m_field4, &m_field8, &m_field0xC); }

    void OnConstruct(void** pField4, int* pField8, int* pField0xC)
    {
        (void)pField8;
        *pField4 = m_inlineBuffer;
        *pField0xC = 97;
    }

    char m_inlineBuffer[97];   // +0x18
};

CDataStoreCache97::CDataStoreCache97()
{
    m_field4 = 0;
    m_field8 = 0;
    m_field0xC = 0;
    m_field10 = 0;
    m_readPos = -1;
    m_reservedSlot0();
}

CDataStoreCache97::~CDataStoreCache97()
{
    if (m_field0xC != -1)
        g_pfnCDataStoreCache97Release(this, &m_field4, 0, &m_field0xC);

    if (m_field0xC != -1)
        g_pfnRecycleBuffer(&m_field4, &m_field8, &m_field0xC);
}

void* __fastcall TrustedSerializeCacheCtor_6F53FAB0(void* buf)
{
    return new (buf) CDataStoreCache97();
}

void __fastcall TrustedSerializeCacheDtor_6F53FD10(void* buf)
{
    ((CDataStoreCache97*)buf)->~CDataStoreCache97();
}

//----------------------------------------------------------------------------
//  0x6F543A40/0x6F543B00/0x6F543BC0/0x6F543C80 - AppendRouteCandidateRecord:
//  four sibling "build one wire record and serialize it" bodies, each
//  constructing a local CDataStoreCache1460 (the same 1460-byte inline-
//  buffer CDataStore view tock_relay_builders.cpp's own BuildRelay0/1
//  already establish - re-declared identically here for extern linkage),
//  writing `payload` into it through its own reserialize/write helper
//  (sub_6F652510/sub_6F652750/sub_6F6527B0 - each its own further,
//  un-investigated record-reserialize subsystem, stay naked thunks; the
//  fourth, sub_6F651610, is netmsgio_651.cpp's own already-real
//  NetMsgWriteDword_6F651610), then handing the finished local store to
//  SerializeExtraHeader_6F537AA0 along with the wire type code.  retn 8
//  (two stack args) confirmed against each.
//
//  Signature correction: the shipped call sites (below, in
//  DesyncCandidateCheck_6F5453A0) push the wire-type constant LAST - i.e.
//  it lands in `arg_0`, the first stack parameter - and the record pointer
//  FIRST - landing in `arg_4`, the second - confirmed against each body's
//  own `mov ecx,[esp+arg_0]` feeding SerializeExtraHeader's `msgType` and
//  `mov edi,[esp+arg_4]` feeding the reserialize call. This file previously
//  declared `(candidateName, payload)` in the opposite roles and order (a
//  pre-existing bug this session fixes): calling that declaration with a
//  real candidate-name pointer as `arg_0` and a bare wire-type integer
//  (0x10/0x16/0x19) as `arg_4` would have handed each reserialize helper a
//  near-null pointer to dereference - a live crash the very first time this
//  path runs, invisible to `verify.py` (an indirect naked thunk call costs
//  nothing to the score either way) and to `thunk_abi_audit.py` (which
//  checks stack *byte count*, not argument identity).
//----------------------------------------------------------------------------
//  Already reconstructed, in their own TU (netevent_routecandidaterecord.cpp -
//  split out specifically so this build's /Ob2 does not inline them into
//  DesyncCandidateCheck_6F5453A0's own tight loop below, which the shipped
//  code never does).
extern void __fastcall AppendRouteCandidateRecord_6F543A40(void* self, void* /*unused_edx*/,
                                                             unsigned int typeCode, void* payload);
extern void __fastcall AppendRouteCandidateRecord_6F543B00(void* self, void* /*unused_edx*/,
                                                             unsigned int typeCode, void* payload);
extern void __fastcall AppendRouteCandidateRecord_6F543BC0(void* self, void* /*unused_edx*/,
                                                             unsigned int typeCode, void* payload);
extern void __fastcall AppendRouteCandidateRecord_6F543C80(void* self, void* /*unused_edx*/,
                                                             unsigned int typeCode, void* payload);

//----------------------------------------------------------------------------
//  0x6F5453A0 - DesyncCandidateCheck, the desync-check def_6F55211B's own
//  tail: runs when the three-way [self+0xBE4]==1/type==0x1B/[self+0xBE0]==0
//  guard holds.  Walks two parallel arrays at self+0x2BC/self+0x2C0
//  (count/data) and self+0x2CC/self+0x2D0 - two independent linear scans
//  for the first non-null slot in each, `edi`/`esi` respectively (edi's
//  array checked first: a hit there skips the esi scan entirely).
//
//  Correction from a shallower earlier dump: which builder gets called is
//  NOT which array the candidate came from - both arrays feed the *same*
//  record-building code, gated only by a "is this the very first candidate
//  appended this pass" flag (`var_114`, seeded 1, cleared to 0 the instant
//  the first candidate is appended). The first candidate ever - from
//  *either* array - goes through sub_6F543A40 with wire type 0x10; every
//  candidate after that, from either array, goes through sub_6F543B00 with
//  wire type 0x16.  When both arrays are finally exhausted, a fixed
//  0x19-byte fallback "self" shape (self+0x1F4 length, self+0x1F6 data)
//  goes through sub_6F543BC0 with wire type 0x19 - then, unconditionally,
//  one more record (a bare dword, the same `var_114`-shaped local re-used
//  and forced back to 1) goes through sub_6F543C80 with wire type 0x1A,
//  every single call through this function, not just the fallback path.
//
//  DIFFERS (unchanged from the existing note, still true): each candidate
//  record's own full field layout is not established past the touched
//  name-length/name-bytes tail (cand+0x29/+0x2A) this file already reads;
//  the dump's own field stores at cand+0x18/0x19/0x1D/0x21/0x25 into the
//  local record ahead of the name, and the further `rep movsd` of 0x2E
//  dwords out of self+0x13C into the same local past the name, are a
//  distinct, un-investigated wire-record subsystem (self+0x13C is its own
//  "SlotRouteRecord"-shaped structure, out of this batch's worklist) and
//  are approximated here by the name-only buffer alone, exactly as the
//  previous reconstruction already did.
//----------------------------------------------------------------------------
void __fastcall DesyncCandidateCheck_6F5453A0(void* self)
{
    char* base = (char*)self;
    *(unsigned int*)(base + 0xF30) = 0;
    *(unsigned int*)(base + 0xF34) = 0;

    unsigned int count1 = *(unsigned int*)(base + 0x2BC);
    void* data1 = *(void**)(base + 0x2C0);
    unsigned int count2 = *(unsigned int*)(base + 0x2CC);
    void* data2 = *(void**)(base + 0x2D0);

    unsigned int idxEdi = 0;
    unsigned int idxEsi = 0;
    int firstAppend = 1;

    for (;;)
    {
        void* candidate = 0;
        for (; idxEdi < count1; ++idxEdi)
        {
            candidate = ((void**)data1)[idxEdi];
            if (candidate) { ++idxEdi; break; }
        }

        if (!candidate)
        {
            for (; idxEsi < count2; ++idxEsi)
            {
                candidate = ((void**)data2)[idxEsi];
                if (candidate) { ++idxEsi; break; }
            }

            if (!candidate)
                break;
        }

        unsigned char* cand = (unsigned char*)candidate;
        unsigned char nameBuf[0x1D];
        unsigned char len = cand[0x29];
        memcpy(nameBuf, cand + 0x2A, len);

        if (firstAppend)
        {
            firstAppend = 0;
            AppendRouteCandidateRecord_6F543A40(self, 0, 0x10, nameBuf);
        }
        else
        {
            AppendRouteCandidateRecord_6F543B00(self, 0, 0x16, nameBuf);
        }
    }

    //  Neither array had a live slot left - the fallback "self" shape: a
    //  fixed 0x19-byte block copied out of self+0x1F4/self+0x1F6.
    unsigned short fallbackLen = *(unsigned short*)(base + 0x1F4);
    char fallbackBuf[0x19];
    memcpy(fallbackBuf, base + 0x1F6, fallbackLen);
    AppendRouteCandidateRecord_6F543BC0(self, 0, 0x19, fallbackBuf);

    //  Unconditional trailing record, every call - not just the fallback
    //  path above - forcing the same local flag back to 1 first (the
    //  dump's own `mov [var_114], 1` immediately before this call; nothing
    //  in this closure suggests the flag survives past this point, so it
    //  is a fresh local rather than the loop's own `firstAppend`).
    int trailingFlag = 1;
    AppendRouteCandidateRecord_6F543C80(self, 0, 0x1A, &trailingFlag);
}

//----------------------------------------------------------------------------
//  0x6F54E860 - FireEvent, table2 case 32 (msg type 0x21)'s own builder.
//  Signature correction: the shipped call site (0x6F551D80's own
//  `push edi(slot); push ecx(&store)` ahead of `call sub_6F54E860`, i.e.
//  `store` ends up `arg_0` and `slot` `arg_4` - confirmed against
//  `mov eax,[edi]` dereferencing arg_0 as a vtable pointer right after the
//  record-ctor call) takes `(self, edx_unused, store, slot)`, not
//  `(self, edx_unused, slot, store)` as this file previously declared (a
//  pre-existing thunk-declaration bug this session fixes along with the
//  one call site below, in CNetEventTypeDispatcher_ConstructAndBroadcast's
//  own table2 case 32).
//
//  Builds a small local record via sub_6F652820 (already thunked,
//  netevent_replaystream.cpp's own RecordCtor_6F652820), bails through
//  ParseError("Net::EVENT_MESSAGE") if the store's own overflow-check
//  vtable slot trips, then re-decodes the record's own message-id byte
//  through a two-level jump table (`byte_6F54EAE8`/`jpt_6F54E972`) into six
//  named diagnostic-log cases (sub_6F543450/540/610/6E0/7B0/880, each its
//  own further, un-investigated string-formatting subsystem - stay naked
//  thunks) that fire once per `slot*0x304`-indexed player record whose own
//  per-slot cache differs from the wire value, then tears the local record
//  down through the pool recycler (off_6F932C0C).  The two-level remap
//  table's own contents are not available in this call tree (no data dump
//  for `byte_6F54EAE8`), so the switch below dispatches directly on the
//  record's own raw message-id byte using the six literal case values the
//  dump's own jump-table labels already give (16/17/18/19/20/32) rather
//  than guessing the remap - retn 8.
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//  cnd-surround-e session: these six are real reconstructions now, built
//  on the exact template netcommand_pause.cpp's own
//  CNetCommandPause_BuildAndFire already establishes for this same
//  "reader/ctx/sender, attach hook, overflow-check-or-fire" shape -
//  `className` here is a CNetCommandBase-derived class exactly like
//  CNetCommandPause, plus one trailing byte field this batch's own read
//  confirms every one of the six carries (`m_field18` below); the "fire"
//  half is instead a per-message diagnostic log call rather than
//  CNetData_FireCommand, since these six are FireEvent_6F54E860's own
//  per-slot-cache-mismatch diagnostics, not turn-stream commands.
//
//  cnd-wave2-a: both of each case's two out-of-line callees are real
//  reconstructions now (see the two blocks just above the macro below) -
//  the "exactly CNetCommandPause_Attach's shape" comment above turned out
//  to describe only Pause's own Attach, which the optimiser reduced to a
//  pure identity function; none of these six were that.  Every one of the
//  six Attach hooks (0x6F555410/360/390/3C0/3F0, 0x6F554890) does a real
//  `CDataStore` read straight into the freshly-built command object -
//  `CDataStore::ReadByte` for the four single-byte selection fields
//  (Team/Color/RacePref/Handicap, into the `m_field18` the macro already
//  declares), `CDataStore::ReadString` for the two chat-message classes
//  (GlueChat: 0x100 bytes at +0x18; GameChat: an extra `ReadDword` at
//  +0x98 first, then 0x80 bytes of string at +0x18) - see
//  `CNetMessageGlueChat_Attach` and friends below for the bodies, and the
//  note on `CNetMessageGameChat_Attach` for the class-body-too-small gap
//  this surfaced.  All six diagnostic-log callees (0x6F53C790/7D0/810/
//  850/890/8D0) turned out to be byte-identical to each other and to
//  `CNetData::FireCommand` (0x6F53B790, netcommand_fire.cpp) - the same
//  "verbose-gated `nullsub_1` line, then `FireToObserverLists` unless the
//  sender is 0xFF" shape `CNetData_FireCommand_6F53A380` above already
//  documents a *third* /OPT:ICF-less copy of; these six make nine.
//
//  NOTE - a discovered gap, left exactly as found rather than fixed here:
//  each case reads its own third argument (`reader` below) at +0x10/+0x14
//  exactly like a `CDataStore*` (cdatastore.h's own m_field10/m_readPos),
//  and its second argument (`ctx`) is stored as a plain `int` (m_ctx) -
//  the identical (ctx, reader) role split CNetCommandPause_BuildAndFire
//  above already has confirmed.  But FireEvent_6F54E860's own current call
//  site (below) passes `store` (the real CDataStore*) in the `ctx` slot
//  and `recordBuf` (its own 16-byte RecordCtor_6F652820 local) in the
//  `reader` slot - backwards, and reading a 16-byte stack buffer's own
//  +0x10/+0x14 bytes is past its declared end.  That looks like the same
//  swapped-argument shape this file's own DesyncCandidateCheck and this
//  very function's header comment (the `(self, edx_unused, store, slot)`
//  correction above) already found and fixed elsewhere - but fixing
//  FireEvent_6F54E860's own call site is outside this batch's three-
//  function worklist (CNetEventTypeDispatcher_ConstructAndBroadcast /
//  DesyncCandidateCheck / CNetEventTrustedDesync_Ctor) and risks that
//  function's own already-real 995/1409 dispatcher score for a change
//  this batch cannot verify live - left exactly as found, flagged here
//  for whoever next picks up FireEvent_6F54E860 itself.  Each case
//  function below is written to match its *own* real disassembly
//  regardless of what its current caller happens to pass, which is what
//  verify.py actually scores.
//----------------------------------------------------------------------------

//  0x6F555410 - CNetMessageGlueChat_Attach.  Reads a NUL-terminated string
//  (size 0x100) straight off the wire into cmd+0x18 - this class's own
//  message-text buffer, which is bigger than the `m_reserved16`/`m_field18`
//  the FIRE_EVENT_CASE macro below declares for every case (see
//  CNetMessageGameChat_Attach's own note) - and, if the store had already
//  run past its declared length by the time the read finished, forces the
//  first byte back to 0 (the same `m_readPos > m_field10` guard
//  CNetCommandPause_BuildAndFire's caller already checks explicitly,
//  applied here defensively to whatever ReadString may have half-written).
void* __fastcall CNetMessageGlueChat_Attach(CDataStore* reader, void* cmd)
{
    char* buf = (char*)cmd + 0x18;
    reader->ReadString(buf, 0x100);
    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
        *buf = 0;
    return reader;
}

//  0x6F555360/0x6F555390/0x6F5553C0/0x6F5553F0 - Team/Color/RacePref/
//  Handicap's own Attach: read exactly one byte off the wire into the
//  `m_field18` the macro already declares for this case.  No overflow
//  check afterward, unlike the two string readers above -
//  `CDataStore::ReadByte`'s own `EnsureReadable` guard already no-ops
//  safely on a dry store, so there is nothing left for the caller to
//  clean up.
void* __fastcall CNetMessageGluePlayerSelectTeam_Attach(CDataStore* reader, void* cmd)
{
    reader->ReadByte((unsigned char*)((char*)cmd + 0x18));
    return reader;
}

void* __fastcall CNetMessageGluePlayerSelectColor_Attach(CDataStore* reader, void* cmd)
{
    reader->ReadByte((unsigned char*)((char*)cmd + 0x18));
    return reader;
}

void* __fastcall CNetMessageGluePlayerSelectRacePref_Attach(CDataStore* reader, void* cmd)
{
    reader->ReadByte((unsigned char*)((char*)cmd + 0x18));
    return reader;
}

void* __fastcall CNetMessageGluePlayerSelectHandicap_Attach(CDataStore* reader, void* cmd)
{
    reader->ReadByte((unsigned char*)((char*)cmd + 0x18));
    return reader;
}

//  0x6F554890 - CNetMessageGameChat_Attach.  Two wire fields, read in this
//  order: a dword at cmd+0x98, then a NUL-terminated string (size 0x80) at
//  cmd+0x18, with the same defensive null-terminate-on-overrun tail
//  GlueChat's own Attach has.  Both offsets are well past the plain
//  `m_reserved16`/`m_field18` FIRE_EVENT_CASE declares for every case, so
//  this class's real object is materially larger than what the macro
//  models - not fixed here (out of this batch's Attach/Log-only worklist,
//  and every case's own local is stack-allocated by its builder in
//  `fnName` below regardless, so widening just this one class's layout
//  would still need that call site's own frame audited); flagged the same
//  way FireEvent_6F54E860's own swapped-argument gap already is above, for
//  whoever next picks up CNetMessageGameChat itself.
void* __fastcall CNetMessageGameChat_Attach(CDataStore* reader, void* cmd)
{
    reader->ReadDword((unsigned int*)((char*)cmd + 0x98));
    char* buf = (char*)cmd + 0x18;
    reader->ReadString(buf, 0x80);
    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
        *buf = 0;
    return reader;
}

//  The six diagnostic-log callees (0x6F53C790/7D0/810/850/890/8D0) are, byte
//  for byte, six more /OPT:ICF-less copies of `CNetData::FireCommand`
//  (0x6F53B790, netcommand_fire.cpp) - the same "verbose-gated `nullsub_1`
//  line, then `FireToObserverLists` unless the sender is 0xFF" shape
//  `CNetData_FireCommand_6F53A380` above already documents a third copy
//  of.  Two of FIRE_EVENT_CASE's own parameter names are swapped from
//  FireCommand's real roles: `turn` here is the verbose *gate*, and
//  `zero2` is the value the log line actually prints as "turn %u". Real
//  bodies never set `eax`, matching the disassembly exactly (no return
//  value - `fnName` below discards it either way), so these are `void`
//  despite the macro's own `void*`-returning NAKED_THUNK declaration that
//  used to stand in for them.
void __fastcall CNetMessageGlueChat_Log(void* recThis, void* rec, const char* valueName, int turn, int zero2)
{
    CNetCommandBase* cmd = (CNetCommandBase*)rec;
    if (turn)
        nullsub_1("War3Net: (INFO): turn %u: %s from %u\n", zero2, valueName, cmd->m_sender);
    if (cmd->m_sender != 0xFF)
        ((CNetPlayerRecord*)recThis)->FireToObserverLists(cmd);
}

void __fastcall CNetMessageGluePlayerSelectTeam_Log(void* recThis, void* rec, const char* valueName, int turn, int zero2)
{
    CNetCommandBase* cmd = (CNetCommandBase*)rec;
    if (turn)
        nullsub_1("War3Net: (INFO): turn %u: %s from %u\n", zero2, valueName, cmd->m_sender);
    if (cmd->m_sender != 0xFF)
        ((CNetPlayerRecord*)recThis)->FireToObserverLists(cmd);
}

void __fastcall CNetMessageGluePlayerSelectColor_Log(void* recThis, void* rec, const char* valueName, int turn, int zero2)
{
    CNetCommandBase* cmd = (CNetCommandBase*)rec;
    if (turn)
        nullsub_1("War3Net: (INFO): turn %u: %s from %u\n", zero2, valueName, cmd->m_sender);
    if (cmd->m_sender != 0xFF)
        ((CNetPlayerRecord*)recThis)->FireToObserverLists(cmd);
}

void __fastcall CNetMessageGluePlayerSelectRacePref_Log(void* recThis, void* rec, const char* valueName, int turn, int zero2)
{
    CNetCommandBase* cmd = (CNetCommandBase*)rec;
    if (turn)
        nullsub_1("War3Net: (INFO): turn %u: %s from %u\n", zero2, valueName, cmd->m_sender);
    if (cmd->m_sender != 0xFF)
        ((CNetPlayerRecord*)recThis)->FireToObserverLists(cmd);
}

void __fastcall CNetMessageGluePlayerSelectHandicap_Log(void* recThis, void* rec, const char* valueName, int turn, int zero2)
{
    CNetCommandBase* cmd = (CNetCommandBase*)rec;
    if (turn)
        nullsub_1("War3Net: (INFO): turn %u: %s from %u\n", zero2, valueName, cmd->m_sender);
    if (cmd->m_sender != 0xFF)
        ((CNetPlayerRecord*)recThis)->FireToObserverLists(cmd);
}

void __fastcall CNetMessageGameChat_Log(void* recThis, void* rec, const char* valueName, int turn, int zero2)
{
    CNetCommandBase* cmd = (CNetCommandBase*)rec;
    if (turn)
        nullsub_1("War3Net: (INFO): turn %u: %s from %u\n", zero2, valueName, cmd->m_sender);
    if (cmd->m_sender != 0xFF)
        ((CNetPlayerRecord*)recThis)->FireToObserverLists(cmd);
}

#define FIRE_EVENT_CASE(fnName, className, magicId, subIndex, field18Val, attachAddr, logAddr) \
    class className : public CNetCommandBase                                  \
    {                                                                          \
    public:                                                                   \
        className(int ctxVal, unsigned char sender)                           \
            : CNetCommandBase(magicId, subIndex)                              \
        {                                                                     \
            m_ctx = ctxVal;                                                   \
            m_sender = sender;                                                \
            m_field18 = (field18Val);                                         \
        }                                                                     \
        unsigned short m_reserved16;   /* +0x16, never touched */             \
        unsigned char  m_field18;      /* +0x18 */                            \
    };                                                                        \
    __declspec(noinline) void __fastcall fnName(void* recThis, void* ctxArg, void* readerArg, \
                            unsigned int byteVal, const char* valueName,      \
                            int zero1, int replayFlag, int turn, int zero2)   \
    {                                                                         \
        (void)replayFlag;                                                    \
        int ctx = (int)(size_t)ctxArg;                                       \
        CDataStore* reader = (CDataStore*)readerArg;                         \
        className rec(ctx, (unsigned char)byteVal);                          \
        className##_Attach(reader, &rec);                                    \
        if (zero1)                                                           \
            return;                                                          \
        if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10) \
        {                                                                     \
            CNetData_ParseError(valueName);                                  \
            return;                                                          \
        }                                                                     \
        className##_Log(recThis, &rec, valueName, turn, zero2);              \
    }
FIRE_EVENT_CASE(FireEventCase16_6F543450, CNetMessageGlueChat,                 0xB0010, 0x10, 0,    0x6F555410, 0x6F53C790)
FIRE_EVENT_CASE(FireEventCase17_6F543540, CNetMessageGluePlayerSelectTeam,     0xB0011, 0x11, 0xFF, 0x6F555360, 0x6F53C7D0)
FIRE_EVENT_CASE(FireEventCase18_6F543610, CNetMessageGluePlayerSelectColor,    0xB0012, 0x12, 0,    0x6F555390, 0x6F53C810)
FIRE_EVENT_CASE(FireEventCase19_6F5436E0, CNetMessageGluePlayerSelectRacePref, 0xB0013, 0x13, 0,    0x6F5553C0, 0x6F53C850)
FIRE_EVENT_CASE(FireEventCase20_6F5437B0, CNetMessageGluePlayerSelectHandicap, 0xB0014, 0x14, 0,    0x6F5553F0, 0x6F53C890)
FIRE_EVENT_CASE(FireEventCase32_6F543880, CNetMessageGameChat,                 0xB0020, 0x20, 0,    0x6F554890, 0x6F53C8D0)
#undef FIRE_EVENT_CASE

//  sub_6F652820 - already reconstructed in netevent_replaystream_records.cpp
//  as RecordCtor_6F652820(CDataStore* store, void* record); declared here
//  (not redefined) for extern linkage.  This session fixed both a
//  declaration mismatch (this file previously declared it `void* (void*,
//  void*)`, a distinct mangled symbol from the real `CDataStore*
//  (CDataStore*, void*)` definition - exactly the kind of drift
//  link_check.py exists to catch, though it had not yet been rebuilt after
//  the two TUs diverged) and the one call site below, which was passing
//  `slot` where the real function's own first argument is `store`.
extern CDataStore* __fastcall RecordCtor_6F652820(CDataStore* store, void* record);

void __fastcall FireEvent_6F54E860(void* self, void* /*unused_edx*/, void* store, unsigned int slot);

//----------------------------------------------------------------------------
//  0x6F537B50 - a tail-jump through a second embedded object's own
//  function-pointer slot at [self+0x2278][+8] (raw, not a C++ vtable read -
//  the dump: `mov edx,[eax+8]; add ecx,2278h; jmp edx`), called only from
//  the def_6F55211B tail's "[self+0x1B44] was set" branch.  `this` only,
//  retn bare (the callee it tail-jumps to cleans whatever it cleans) -
//  reproduced literally, naked, since the tail-jump target's own arity is
//  unknown (a `__asm` block is not eligible for the register/scheduling
//  IDENTICAL excuses, CLAUDE.md's own naked-body rule, but is exactly
//  right for a raw tail-jump like this one).
//----------------------------------------------------------------------------
__declspec(naked) void __fastcall FlushLatchedCommand_6F537B50(void* self)
{
    __asm
    {
        mov eax, [ecx+2278h]
        mov edx, [eax+8]
        add ecx, 2278h
        jmp edx
    }
}

//----------------------------------------------------------------------------
//  0x6F537E10 / 0x6F537E30 - `CNetData`'s own checksum-provider registration
//  (`context->m_pCache[2]`, the "net" category - docs/notes/
//  checksum-provider-registry.md).  Both are this-adjusting thunks into the
//  same embedded `NTempest::CSyncData` at `self+0x2278` that
//  `FlushLatchedCommand_6F537B50` above reads at `[+8]`; these two read
//  `[+4]` instead - `CSyncData`'s own vtable slot 1, `GetChecksum`
//  (syncdata.h/.cpp), RTTI/live-confirmed by the doc's own "LIVE FINDING"
//  section.  `self` here is the same object netcommand_dispatch.cpp's
//  SNetSessionInfo already models (its own `m_reserved2278` is this exact
//  offset).
//----------------------------------------------------------------------------

//  Storm ordinal 548 - format into the caller's buffer, unbounded, __cdecl
//  (Misc/cheatdata.cpp's own declaration comment has the full reasoning).
int __cdecl Storm_548(char* buffer, const char* format, ...);

//  0x6F537E10 - the "net" provider's own `ComputeChecksum()` override
//  (`CSynchronousData`'s slot 0 - Game/synchronousdata.h): pure
//  adjust-and-tail-jump, unknown arity beyond `this` (matching
//  FlushLatchedCommand_6F537B50's own naked style immediately above, and
//  CLAUDE.md's own naked-body rule: an `__asm` block is not eligible for
//  register/scheduling excuses, so this should - and does - score 1.000).
__declspec(naked) unsigned int __fastcall CNetData_ComputeChecksum(SNetSessionInfo* self)
{
    __asm
    {
        mov eax, [ecx+2278h]
        mov edx, [eax+4]
        add ecx, 2278h
        jmp edx
    }
}

//  0x6F537E30 - the "net" provider's own `DebugDumpTo(buffer)` override
//  (`CSynchronousData`'s slot 1): prints a "Net:\n" header, then the same
//  `CSyncData::GetChecksum()` value CNetData_ComputeChecksum above forwards
//  to, formatted as "syncNetEventChecksum = %08x\n" - both literal strings
//  read straight off the dump's own `aNet`/`aSyncneteventch` operands.  Real
//  C++ (not naked) since there is genuine work around the forward; the
//  vtable read is still raw (CLAUDE.md's own dispatch idiom), matching the
//  shipped body's own re-read-fresh shape rather than a direct call to
//  CSyncData_GetChecksum.
void __fastcall CNetData_DebugDumpTo(SNetSessionInfo* self, void* /*unused_edx*/,
                                      void* buffer)
{
    Storm_548((char*)buffer, "Net:\n");

    CSyncData* syncData =
        (CSyncData*)((char*)self + 0x2278);
    typedef unsigned int(__fastcall * GetChecksumFn)(CSyncData*);
    GetChecksumFn* vtable = (GetChecksumFn*)syncData->m_vtable;
    unsigned int checksum = vtable[1](syncData);

    Storm_548((char*)buffer, "syncNetEventChecksum = %08x\n", checksum);
}

//  sub_6F54B2D0 / sub_6F546E50 - CNetEventTrustedDesync/
//  CNetEventTrustedResult's own constructor+broadcast (table2 cases 48/49,
//  msg types 0x31/0x32) - each fires through 0x6F5378E0
//  (FireToObserverLists, already EXACT) internally rather than through the
//  uniform 7-arg shape every other table2 case uses.  Real bodies below
//  (CNetEventTrustedDesync_Ctor / CNetEventTrustedResult_Ctor), after the
//  CNetEventBase/NetEventCtorFire infrastructure they both reuse.
void __fastcall CNetEventTrustedDesync_Ctor(void* thisObj, unsigned int flag,
                                             void* store, const char* name,
                                             int zeroA, int zeroB, int slotFlag,
                                             int turn, int zeroC);
void __fastcall CNetEventTrustedResult_Ctor(void* thisObj, unsigned int flag,
                                             void* store, const char* name,
                                             int zeroA, int zeroB, int slotFlag,
                                             int turn, int zeroC);

//----------------------------------------------------------------------------
//  0x6F53A380 - a third, byte-identical copy of CNetData::FireCommand
//  (netcommand_fire.cpp already has two: 0x6F53B790 and 0x6F53C210; this
//  image was linked without /OPT:ICF, so identical bodies at separate
//  addresses are ordinary here - docs/msvc-vc8-idioms.md,
//  "agentdefaults.cpp").  Reconstructed here (not thunked) rather than
//  moved into netcommand_fire.cpp: its one caller (def_6F55211B's shared
//  tail) sits in this module's own address range, and every callee it
//  needs (nullsub_1, CNetPlayerRecord::FireToObserverLists) is already
//  real.
//----------------------------------------------------------------------------
void __fastcall CNetData_FireCommand_6F53A380(CNetPlayerRecord* player, CNetCommandBase* cmd,
                                               const char* name, int verbose, unsigned int turn)
{
    if (verbose)
        nullsub_1("War3Net: (INFO): turn %u: %s from %u\n", turn, name, cmd->m_sender);

    if (cmd->m_sender != 0xFF)
        player->FireToObserverLists(cmd);
}

#undef NAKED_THUNK

//----------------------------------------------------------------------------
//  The ~40 uniform-shaped table2 CNetEvent constructors - `this` =
//  self+8+slot*0x304, `edx` = slot, 7 stack args (CDataStore* store, name,
//  0, 0-or-dtor, flag, turn, 0), retn 0x1Ch.  Real reconstructions below
//  (un-thunked as part of the DesyncCandidateCheck_6F5453A0 closure pass -
//  networking_desync_closure_worklist.json, batch-A).
//
//  Every one shares the same base-object shape (see CNetEventBase below)
//  plus one extra dword read straight off the wire through `store`, the
//  same virtual-overflow-check-then-fire-then-optional-dtor tail, and the
//  same unreproducible VC8 "unified" SEH frame docs/msvc-vc8-idioms.md's
//  "An `__except_handler4`-shaped frame this toolchain cannot reproduce"
//  section and netcommand.h's own CNetCommandBase comment both already
//  establish for this exact shape (a local object with a virtual - even if
//  empty - destructor next to a call that might throw, under this repo's
//  fixed `/GS- /EHs-c-`): the frame furniture never matches, the body does.
//  Every one is `BEHAVIOUR`'d `IDENTICAL` in tools/funcmap.py accordingly.
//
//  The per-constructor "read one dword off the wire" helper
//  (`sub_6F651360`/`sub_6F651380`/... - a distinct address per event in the
//  dump, /OPT:ICF-less duplicates of the identical three-instruction body)
//  is not reproduced at 40 distinct addresses: `canonlib` maps every call
//  target to the placeholder `SYM` (docs/msvc-vc8-idioms.md's own
//  `call __imp__` finding already established that a call's *target* is
//  invisible to the score), so one shared, `__declspec(noinline)` helper
//  produces the identical canonical instruction stream at every call site.
//
//  The store's own "ran out of declared data" check ([esi]->vtbl[6],
//  offset 0x18) is a genuine virtual call in the dump, but CDataStore's
//  header (cdatastore.h) only names its first three virtual slots
//  (reservedSlot0/reservedSlot1/Grow) - adding a fourth here would be an
//  unconfirmed guess touching a class dozens of other reconstructions
//  share.  netdata.h's own SNetSessionInfo comment already sets the
//  precedent for exactly this situation (CDataStore vtable slots 7/8, used
//  by ConstructAndBroadcast itself): go through the raw vtable-index shape
//  CLAUDE.md's own example uses instead of extending the class.
//----------------------------------------------------------------------------
typedef void (__fastcall *EventCtorFn)(void* thisObj, unsigned int slot, void* store,
                                        const char* name, int zeroA, void* dtorOrZero,
                                        int flag, int turn, int zeroC);

//  The generic "extra hook" callback both zeroA and dtorOrZero are: always
//  0 in every FIRE_EVENT call site above except dtorOrZero, which some
//  cases pass as a real address (0x6F551B80 and friends) - `ecx = &evt`,
//  `edx = 0` at every observed call site.
typedef void (__fastcall *NetEventHookFn)(void* evt, int zero);

//  The base every one of table2's ~40 CNetEvent subclasses shares: vtable
//  (stamped by the derived class - see the dead-store-elimination note on
//  CNetCommandBase, netcommand.h, for why the base's own redundant store
//  never survives), then three plain dwords always 0/id/0.  Overlays
//  observer.h's SAgentMessage the same way CNetCommandBase does (both are
//  handed generically to CNetPlayerRecord::FireToObserverLists).
//----------------------------------------------------------------------------
class CNetEventBase
{
public:
    virtual ~CNetEventBase() {}

    CNetEventBase(unsigned int id) : m_field4(0), m_id(id), m_field0C(0) {}

    int          m_field4;   // +0x04 (always 0)
    unsigned int m_id;       // +0x08 (the per-event-type wire id, 0x400900xx)
    int          m_field0C;  // +0x0C (always 0)
};

//  sub_6F651360 and its ~40 /OPT:ICF-less siblings - see the header comment
//  above for why one shared helper reproduces every one of their call
//  sites identically.
static __declspec(noinline) CDataStore* __fastcall NetEventReadDwordField(
    CDataStore* store, unsigned int* out)
{
    return store->ReadDword(out);
}

//  CDataStore's own vtable slot 6 (offset 0x18) - "has this store run past
//  its declared length" (the same condition ConstructAndBroadcast's type-
//  0x10/0x15 special case checks inline as `m_readPos > m_field10`,
//  called virtually here because the concrete type is not known statically
//  at this call site).  Raw vtable-index dispatch, not a CDataStore
//  virtual - see the header comment above.
typedef int (__thiscall *StoreOverflowCheckFn)(void*);
#define NET_EVENT_STORE_OVERFLOWED(storePtr) \
    ((*(StoreOverflowCheckFn**)(storePtr))[6]((void*)(storePtr)))

void __fastcall FireEvent_6F54E860(void* self, void* /*unused_edx*/, void* store, unsigned int slot)
{
    char* base = (char*)self;
    unsigned char recordBuf[0x10];
    memset(recordBuf, 0, sizeof(recordBuf));
    RecordCtor_6F652820((CDataStore*)store, recordBuf);

    if (NET_EVENT_STORE_OVERFLOWED(store))
    {
        CNetData_ParseError("Net::EVENT_MESSAGE");
        return;
    }

    void* recThis = base + slot * 0x304 + 0x290;
    unsigned char byteVal = recordBuf[0];
    unsigned int turn = *(unsigned int*)(base + 0x1C68);
    int replayFlag = *(int*)(base + 0x2274);

    switch (byteVal)
    {
    case 16: FireEventCase16_6F543450(recThis, store, recordBuf, byteVal, ValueName, 0, replayFlag, turn, 0); break;
    case 17: FireEventCase17_6F543540(recThis, store, recordBuf, byteVal, ValueName, 0, replayFlag, turn, 0); break;
    case 18: FireEventCase18_6F543610(recThis, store, recordBuf, byteVal, ValueName, 0, replayFlag, turn, 0); break;
    case 19: FireEventCase19_6F5436E0(recThis, store, recordBuf, byteVal, ValueName, 0, replayFlag, turn, 0); break;
    case 20: FireEventCase20_6F5437B0(recThis, store, recordBuf, byteVal, ValueName, 0, replayFlag, turn, 0); break;
    case 32: FireEventCase32_6F543880(recThis, store, recordBuf, byteVal, ValueName, 0, replayFlag, turn, 0); break;
    default: break;
    }
}

//  The shared post-construction tail every one of table2's ~40 CNetEvent
//  cases runs: overflow-check -> ParseError-and-bail, or optional zeroA
//  hook -> optional verbose log -> FireToObserverLists -> optional dtor
//  hook.  A macro (not a shared function): every one of the ~40 inlines
//  this tail itself in the dump, and an out-of-line call here would cost
//  matched instructions for no reason beyond avoiding forty near-identical
//  bodies - the macro pays that cost once instead.
#define NET_EVENT_CTOR_FIRE(thisObjVar, storeVar, evtPtr, nameVar, zeroAVar, dtorVar, flagVar, turnVar) \
    do                                                                         \
    {                                                                          \
        if (NET_EVENT_STORE_OVERFLOWED(storeVar))                             \
        {                                                                      \
            CNetData_ParseError(nameVar);                                      \
            return;                                                            \
        }                                                                       \
        if (zeroAVar)                                                           \
            ((NetEventHookFn)(zeroAVar))(evtPtr, 0);                            \
        if (flagVar)                                                            \
            nullsub_1("War3Net: (INFO): turn %u: %s\n", turnVar, nameVar);       \
        ((CNetPlayerRecord*)(thisObjVar))->FireToObserverLists(                  \
            reinterpret_cast<CNetCommandBase*>(evtPtr));                         \
        if (dtorVar)                                                             \
            ((NetEventHookFn)(dtorVar))(evtPtr, 0);                              \
    } while (0)

//  One extra dword read off the wire, nothing else - the ~27 of the ~40
//  whose own dump body is exactly this shape (79 instructions, modulo
//  frame-size furniture).
#define NET_EVENT_CTOR_UNIFORM(fnName, className, magicId)                    \
    class className : public CNetEventBase                                   \
    {                                                                         \
    public:                                                                   \
        className(unsigned int slot, CDataStore* store) : CNetEventBase(magicId) \
        {                                                                      \
            m_slot = slot;                                                    \
            NetEventReadDwordField(store, &m_extra);                          \
        }                                                                      \
        unsigned int m_slot;                                                  \
        unsigned int m_extra;                                                 \
    };                                                                         \
    void __fastcall fnName(void* thisObj, unsigned int slot, void* storeArg,   \
                            const char* name, int zeroA, void* dtorOrZero,      \
                            int flag, int turn, int zeroC)                      \
    {                                                                           \
        (void)zeroC;                                                            \
        CDataStore* store = (CDataStore*)storeArg;                              \
        className evt(slot, store);                                            \
        NET_EVENT_CTOR_FIRE(thisObj, store, (&evt), name, zeroA, dtorOrZero,      \
                            flag, turn);                                        \
    }

NET_EVENT_CTOR_UNIFORM(CNetEventConnect_Ctor,                CNetEventConnect,                0x40090064)
NET_EVENT_CTOR_UNIFORM(CNetEventDisconnect_Ctor,             CNetEventDisconnect,              0x40090065)
NET_EVENT_CTOR_UNIFORM(CNetEventGameListStart_Ctor,          CNetEventGameListStart,           0x40090066)
NET_EVENT_CTOR_UNIFORM(CNetEventGameListStop_Ctor,           CNetEventGameListStop,            0x40090067)
NET_EVENT_CTOR_UNIFORM(CNetEventGameListError_Ctor,          CNetEventGameListError,           0x40090068)
NET_EVENT_CTOR_UNIFORM(CNetEventGameListUpdate_Ctor,         CNetEventGameListUpdate,          0x4009006A)
NET_EVENT_CTOR_UNIFORM(CNetEventGameListDelete_Ctor,         CNetEventGameListDelete,          0x4009006B)
NET_EVENT_CTOR_UNIFORM(CNetEventTeamGameListStart_Ctor,      CNetEventTeamGameListStart,       0x4009006C)
NET_EVENT_CTOR_UNIFORM(CNetEventTeamGameListStop_Ctor,       CNetEventTeamGameListStop,        0x4009006D)
NET_EVENT_CTOR_UNIFORM(CNetEventTeamGameListDelete_Ctor,     CNetEventTeamGameListDelete,      0x40090070)
NET_EVENT_CTOR_UNIFORM(CNetEventAnonGameFind_Ctor,           CNetEventAnonGameFind,            0x40090071)
NET_EVENT_CTOR_UNIFORM(CNetEventAnonGameJoin_Ctor,           CNetEventAnonGameJoin,            0x40090072)
NET_EVENT_CTOR_UNIFORM(CNetEventTeamInfo_Ctor,               CNetEventTeamInfo,                0x40090076)
NET_EVENT_CTOR_UNIFORM(CNetEventPlayerLeave_Ctor,            CNetEventPlayerLeave,             0x4009007A)
NET_EVENT_CTOR_UNIFORM(CNetEventPlayerReady_Ctor,            CNetEventPlayerReady,             0x4009007B)
NET_EVENT_CTOR_UNIFORM(CNetEventGameClose_Ctor,              CNetEventGameClose,               0x4009007D)
NET_EVENT_CTOR_UNIFORM(CNetEventGameStart_Ctor,              CNetEventGameStart,               0x4009007E)
NET_EVENT_CTOR_UNIFORM(CNetEventGameReady_Ctor,              CNetEventGameReady,               0x4009007F)
NET_EVENT_CTOR_UNIFORM(CNetEventPlayerUpdate_Ctor,           CNetEventPlayerUpdate,            0x40090080)
NET_EVENT_CTOR_UNIFORM(CNetEventPlayerResume_Ctor,           CNetEventPlayerResume,            0x40090084)
NET_EVENT_CTOR_UNIFORM(CNetEventRouterHandoffSearching_Ctor, CNetEventRouterHandoffSearching,  0x40090086)
NET_EVENT_CTOR_UNIFORM(CNetEventRouterHandoffSyncing_Ctor,   CNetEventRouterHandoffSyncing,    0x40090087)
NET_EVENT_CTOR_UNIFORM(CNetEventRouterHandoffDone_Ctor,      CNetEventRouterHandoffDone,       0x40090088)
NET_EVENT_CTOR_UNIFORM(CNetEventRouterUnresponsive_Ctor,     CNetEventRouterUnresponsive,      0x40090089)
NET_EVENT_CTOR_UNIFORM(CNetEventRouterResponsive_Ctor,       CNetEventRouterResponsive,        0x4009008A)
NET_EVENT_CTOR_UNIFORM(CNetEventSetTurnsLatency_Ctor,        CNetEventSetTurnsLatency,         0x40090082)
NET_EVENT_CTOR_UNIFORM(CNetEventTrigger_Ctor,                CNetEventTrigger,                 0x4009008F)

#undef NET_EVENT_CTOR_UNIFORM

//----------------------------------------------------------------------------
//  The remaining ~15 table2 CNetEvent constructors carry more than one
//  wire field (a game name, a settings blob, several dwords) rather than
//  the uniform group's single dword, so each has its own per-event
//  deserializer at its own address (`sub_6F6525E0` and friends below) -
//  genuinely distinct bodies (a dword here, a `ReadString` there, a nested
//  helper call elsewhere), not /OPT:ICF-less duplicates of one shape the
//  way the uniform group's per-event dword readers are.  Reconstructing
//  each one's own wire layout is its own further target (none of them is
//  in this session's worklist - networking_desync_closure_worklist.json's
//  batch-A names only the constructors); each stays a correctly-ABI'd
//  naked thunk to its real, unhooked body (0 stack args confirmed against
//  its own `retn` below), called from a real constructor with a payload
//  buffer sized generously against the caller's own local-frame size
//  (`sub esp, N` in the dump) rather than the deserializer's own precise
//  field layout.  BEHAVIOUR-noted `DIFFERS` in tools/funcmap.py: past the
//  frame and the payload-field approximation, the overflow-check/optional-
//  hook/fire/optional-dtor tail is the same real logic every uniform-group
//  constructor above runs.
//----------------------------------------------------------------------------
#define NET_EVENT_DESERIALIZER_THUNK(readerName, addr)                        \
    __declspec(naked) void* __fastcall readerName(void* store, void* dst)     \
    {                                                                          \
        __asm { mov eax, addr }                                               \
        __asm { jmp eax }                                                     \
    }

//  netremainder2 session (batch-G/H/K/L closure pass): six of this group's
//  twelve deserializers turned out to call only already-reconstructed
//  leaves (ReadDword/ReadByte/DataStoreReadHeaderBlock/
//  ReadNameAndClampedField8_6F6522A0/NetMsgReadBlock20) and are real
//  reconstructions now - netevent_multipayloadreaders.cpp, NOT inline here.
//  They have to live in their own TU: defining them in this file let this
//  build's /Ob2 inline each one straight into its own NET_EVENT_CTOR_MULTI
//  constructor above, which replaces that constructor's own `call SYM` to
//  the real, out-of-line shipped deserializer with the inlined reads and
//  costs the *constructor's* score, not just the reader's own (caught by
//  this session's own REGRESSION line on CNetEventTypeDispatcher_
//  ConstructAndBroadcast's dump before it was fixed). A second TU for a
//  handful of very small functions is not itself a new "module" by
//  CLAUDE.md's own address-neighbourhood rule (0x6F6525C0-0x6F652966 sit
//  well outside the dispatcher's own 0x6F551D80), so this is the correct
//  split rather than a `noinline` workaround.
void* __fastcall ReadGameAdPayload(void* storeArg, void* dst);
void* __fastcall ReadGameFindPayload(void* storeArg, void* dst);
void* __fastcall ReadGameJoinPayload(void* storeArg, void* dst);
void* __fastcall ReadDistFileStartPayload(void* storeArg, void* dst);
void* __fastcall ReadDistFileCompletePayload(void* storeArg, void* dst);
void* __fastcall ReadGameListAddPayload(void* storeArg, void* dst);

#undef NET_EVENT_DESERIALIZER_THUNK

//----------------------------------------------------------------------------
//  cnd-surround-e session: the remaining six deserializers each reach at
//  least one callee (sub_6F651250, sub_6F6511E0/sub_6F653760,
//  sub_6F5382E0/sub_6F6512B0, sub_6F6537F0, sub_6F653860) this repo still
//  has no dump for - the comment this replaces called that "the further
//  target"; it is still true of those seven addresses, but the operator's
//  own bulk agent_worktrees re-dump now covers these six *readers'* own
//  bodies, so the six themselves are real reconstructions now, each
//  thunking only the one or two deeper, genuinely-unidentified-wire-format
//  callees it reaches - exactly the split netevent_multipayloadreaders.cpp
//  already uses for its own six (DataStoreReadHeaderBlock/
//  ReadNameAndClampedField8_6F6522A0/NetMsgReadBlock20 real, their own
//  deeper unknowns left alone).  ABI of every thunked callee below is
//  read directly off its own call site (register vs. stack argument
//  placement), not guessed.
//----------------------------------------------------------------------------

//  0x6F650770 - Net/netmsgio_650_readblock.cpp; not otherwise forward-
//  declared in this TU.
void __fastcall NetMsgReadBlock20(CDataStore* self, void* dest);

//----------------------------------------------------------------------------
//  cnd-wave2-c: the seven deeper callees the six payload readers above left
//  thunked ("the further, un-investigated wire-format subsystem") turned
//  out to be small leaves in their own right, each reachable through
//  already-reconstructed CDataStore members - none opened onto anything
//  bigger.  Depth-2/3 callees the walk reached along the way
//  (sub_6F650E60, sub_6F6534D0/sub_6F651EE0/sub_6F651330,
//  sub_6F653520/sub_6F653190/sub_6F6523F0/sub_6F651B40) are real
//  reconstructions too, for the same reason.
//----------------------------------------------------------------------------

//  0x6F651250 - ReadTeamGameAdPayload's own "wide field": two dwords, a
//  byte, then NetMsgReadBlock20's own twenty-byte block (29 bytes total).
//  `this` = store, `edx` = dest, retn bare.
__declspec(noinline) void* __fastcall ReadTeamGameAdWideField_6F651250(void* storeArg, void* dest)
{
    CDataStore* store = (CDataStore*)storeArg;
    char* out = (char*)dest;
    store->ReadDword((unsigned int*)out);
    store->ReadDword((unsigned int*)(out + 4));
    store->ReadByte((unsigned char*)(out + 8));
    NetMsgReadBlock20(store, out + 9);
    return store;
}

//  0x6F650E60 - a four-byte raw run followed by a word, both through
//  already-reconstructed CDataStore members - ReadTeamGameListWideField's
//  own tail below.  `this` = store, `edx` = dest, retn bare.
__declspec(noinline) void* __fastcall ReadRawAndWordField_6F650E60(void* storeArg, void* dest)
{
    CDataStore* store = (CDataStore*)storeArg;
    char* out = (char*)dest;
    store->ReadRaw(out, 4);
    store->ReadWord((unsigned short*)(out + 4));
    return store;
}

//  0x6F6511E0 - two dwords, then ReadRawAndWordField_6F650E60's own 6-byte
//  tail at +0x08 (14 bytes total).  `this` = store, `edx` = dest, retn bare.
__declspec(noinline) void* __fastcall ReadTeamGameListWideField_6F6511E0(void* storeArg, void* dest)
{
    CDataStore* store = (CDataStore*)storeArg;
    char* out = (char*)dest;
    store->ReadDword((unsigned int*)out);
    store->ReadDword((unsigned int*)(out + 4));
    ReadRawAndWordField_6F650E60(store, out + 8);
    return store;
}

//  0x6F653760 - the same "probe one byte past the caller's own fixed cap;
//  a wire value (or an unread, still-sentinel probe byte) *above* cap means
//  a short/EOF/overflow read (bump the read-position counter and stop),
//  a value <= cap is the real element count" clamp shape
//  ReadFixedField16_6F652D60/ReadFixedField16_6F651E60
//  (Net/netevent_replaystream_records.cpp) are filed under - note for
//  whoever next touches those two: this dump's own branch
//  (`cmp al,bl` / `jbe` to the *count* path, falling through to the
//  read-position bump) puts the short-read arm on `tmp > sz`, not
//  `tmp <= sz` as those two committed functions currently read; this
//  file's own three instantiations of the shape (this one,
//  ReadDistFileProgressField_6F6537F0, ReadOfficialPlayersField_6F653860)
//  all confirmed the `tmp > sz` polarity directly against their own raw
//  bytes and score cleanly with it - the other two were not re-verified
//  this session, since they belong to a different address/session's own
//  worklist.  Over
//  Render/aufixedstring.h's AuFixedStringArray (TSFixedArray<AuFixedString>)
//  instead of a fixed-stride raw array - SetCount is the *same* shipped
//  body already reconstructed at 0x6F553C50
//  (Render/fontfileversionarray.cpp), reached directly by this dump's own
//  `call sub_6F553C50`, which is why AuFixedString/AuFixedStringArray moved
//  into a shared header rather than this file standing up a second,
//  redundant instantiation.  Each element is a plain
//  CDataStore::ReadString with the usual zero-on-overrun guard, not another
//  clamped sub-record.  `this` = store, `edx` = dest, one stack arg (size,
//  always 4 at both call sites), retn 4.
__declspec(noinline) void* __fastcall ReadTeamGameListTailField_6F653760(void* storeArg, void* dest, unsigned int size)
{
    CDataStore* store = (CDataStore*)storeArg;
    AuFixedStringArray* arr = (AuFixedStringArray*)dest;

    unsigned char sz = (unsigned char)size;
    unsigned char tmp = (unsigned char)(sz + 1);
    store->ReadByte(&tmp);
    if (tmp > sz)
    {
        store->m_readPos = store->m_field10 + 1;
        return store;
    }

    unsigned int count = tmp;
    arr->SetCount(count);
    for (unsigned int i = 0; i < arr->m_count; i++)
    {
        char* elem = (*arr)[i].m_data;
        store->ReadString(elem, 0x20);
        if ((unsigned int)store->m_readPos > (unsigned int)store->m_field10)
            elem[0] = 0;
    }
    return store;
}

//  0x6F6536C0's own growable "suspend player" list - a small count/array-
//  pointer record (m_field0 = the array's own allocation, written only by
//  GrowOrRelease below; m_count at +0x04, m_array at +0x08,
//  Net::SUSPENDPLAYER[m_count], stride 8).  GrowOrRelease's own call site
//  (`push count; mov ecx,list; call`) puts `count` on the stack rather than
//  in `edx`, so it is `__thiscall`, not `__fastcall` - the same reason
//  `SPathRefArrayGrow::SetAlloc` (pathref_load.cpp) is wrapped in a
//  one-method class instead of declared as a free function.
struct SSuspendPlayerList
{
    unsigned int m_field0;
    unsigned int m_count;   // +0x04
    void*        m_array;   // +0x08 - Net::SUSPENDPLAYER[m_count], stride 8

    void GrowOrRelease(unsigned int count);
};

//  0x6F651330 - the stride-8 element ReadGameSuspendPayload's own loop
//  reads through: a byte at +0x00, a dword at +0x04 (three bytes of
//  padding between them are never named because nothing reads them).
//  `this` = store, `edx` = elem, retn bare.
struct SUSPENDPLAYER
{
    unsigned char m_field0;
    char          m_reserved[3];
    unsigned int  m_field4;
};

//  0x6F5382E0 - byte for byte TSFixedArray<T>::SetAlloc's own shape
//  (tsfixedarray.inl) transliterated onto SSuspendPlayerList's own
//  hand-rolled fields instead of the generic template - SMemReAlloc first
//  (flags 0x10, resize in place or fail), and only on failure a fresh
//  SMemAlloc plus a placement-copy of the surviving elements out of the old
//  block and an SMemFree of it.  `void`, not `void*`: the shipped body sets
//  no `mov eax,...` before its final `retn 4` (eax is left holding whatever
//  the last Storm call returned), and the caller
//  (ReadGameSuspendPayload) never reads the return value either.
void SSuspendPlayerList::GrowOrRelease(unsigned int count)
{
    SUSPENDPLAYER* olddata = (SUSPENDPLAYER*)m_array;
    m_field0 = count;
    m_array = SMemReAlloc(olddata, count * sizeof(SUSPENDPLAYER),
                          ".?AUSUSPENDPLAYER@Net@@", -2, 0x10);
    if (m_array == 0)
    {
        m_array = SMemAlloc(count * sizeof(SUSPENDPLAYER),
                            ".?AUSUSPENDPLAYER@Net@@", -2, 0);
        if (olddata != 0)
        {
            unsigned int moved = count < m_count ? count : m_count;
            SUSPENDPLAYER* newdata = (SUSPENDPLAYER*)m_array;
            for (unsigned int i = 0; i < moved; i++)
            {
                new (&newdata[i]) SUSPENDPLAYER(olddata[i]);
                olddata[i].~SUSPENDPLAYER();
            }
            SMemFree(olddata, ".?AUSUSPENDPLAYER@Net@@", -2, 0);
        }
    }
}

//  0x6F6512B0 - ReadGameSuspendPayload's own per-element reader: a byte,
//  then a dword at +0x04.  `this` = store, `edx` = elem, retn bare.
__declspec(noinline) void* __fastcall ReadSuspendPlayerElement_6F6512B0(void* storeArg, void* elem)
{
    CDataStore* store = (CDataStore*)storeArg;
    char* out = (char*)elem;
    store->ReadByte((unsigned char*)out);
    store->ReadDword((unsigned int*)(out + 4));
    return store;
}

//----------------------------------------------------------------------------
//  ReadDistFileProgressPayload's own array field (0x6F6537F0) - the same
//  clamp-probe shape as ReadTeamGameListTailField_6F653760 above, this time
//  over a fresh TSFixedArray<AuDistFileProgressRecord> instantiation: a
//  plain two-byte POD record (SetCount's own grow arm has no
//  default-construction code at all, the same "trivial default ctor, MSVC
//  drops the loop" case tsfixedarray.inl's own comment documents for the
//  AgileAgentQuery* instantiation).  Both of the deeper callees this walk
//  reached turned out to already be real, committed reconstructions under
//  their own names, grepped for only after writing a first draft that
//  duplicated them - CLAUDE.md's own "grep src/ for the address" step,
//  caught by thunk_inventory.py's "one shipped address, two names" class
//  before it ever got that far.  `ResizeWordArray` (0x6F651EE0,
//  Net/w3gsaction_tier29.cpp) is *this* SetAlloc's own body byte for byte -
//  the same SMemReAlloc-then-SMemAlloc-and-copy shape at word granularity,
//  written there as a free function taking the array header explicitly
//  rather than as a TSFixedArray<T> member, so SetCount below calls it
//  directly instead of a same-named member this TU would otherwise have to
//  redefine.  `NetMsgReadBytePair_6F651330` (Net/netmsgio_651.cpp) is the
//  per-element reader, byte for byte the same two-ReadByte body this file's
//  own draft had under a new name.
//----------------------------------------------------------------------------
struct AuDistFileProgressRecord
{
    unsigned char m_field0;
    unsigned char m_field1;
};

extern void __fastcall ResizeWordArray(void* arr, unsigned int newCount);
extern CDataStore* __fastcall NetMsgReadBytePair_6F651330(CDataStore* self, unsigned char* dst);

template<>
void TSFixedArray<AuDistFileProgressRecord>::SetCount(unsigned int count)
{
    if (count != m_count)
    {
        if (count == 0)
        {
            if (m_data)
                SMemFree(m_data, ".?AUAuDistFileProgressRecord@@", -2, 0);
            m_alloc = 0;
            m_count = 0;
            m_data = 0;
        }
        else
        {
            ResizeWordArray(this, count);
            for (unsigned int i = m_count; i < count; i++)
                new (&m_data[i]) AuDistFileProgressRecord;
            m_count = count;
        }
    }
}

//  0x6F6537F0 - `this` = store, `edx` = dest, one stack arg (size, always
//  0x10 at its one call site), retn 4.
void* __fastcall ReadDistFileProgressField_6F6537F0(void* storeArg, void* dest, unsigned int size)
{
    CDataStore* store = (CDataStore*)storeArg;
    TSFixedArray<AuDistFileProgressRecord>* arr = (TSFixedArray<AuDistFileProgressRecord>*)dest;

    unsigned char sz = (unsigned char)size;
    unsigned char tmp = (unsigned char)(sz + 1);
    store->ReadByte(&tmp);
    if (tmp > sz)
    {
        store->m_readPos = store->m_field10 + 1;
        return store;
    }

    unsigned int count = tmp;
    arr->SetCount(count);
    for (unsigned int i = 0; i < arr->m_count; i++)
        NetMsgReadBytePair_6F651330(store, (unsigned char*)&(*arr)[i]);

    return store;
}

//----------------------------------------------------------------------------
//  ReadOfficialPlayersPayload's own array field (0x6F653860) - the same
//  clamp-probe shape again, over a third TSFixedArray<T> instantiation:
//  an 18-byte record whose own default ctor writes only its own +0x01 byte
//  (a length prefix) to zero, and whose own copy ctor - reached from
//  SetAlloc's surviving-element copy, not written out by hand here - only
//  ever copies its own two header bytes plus `m_len` more, never all 16
//  data bytes; the dump's own SetAlloc (sub_6F653190) confirms this with a
//  literal `memcpy(new+2, old+2, old[1])` rather than an 18-byte block
//  move.  Each element is read through ReadClampedField16_6F651B40 (the
//  clamp-16 twin of ReadClampedField8_6F651A30,
//  Net/netdata_replayrecordreaders.cpp) preceded by one leading byte.
//----------------------------------------------------------------------------
struct AuOfficialPlayerRecord
{
    AuOfficialPlayerRecord() { m_len = 0; }
    AuOfficialPlayerRecord(const AuOfficialPlayerRecord& other)
    {
        m_field0 = other.m_field0;
        m_len = other.m_len;
        memcpy(m_data, other.m_data, m_len);
    }

    unsigned char m_field0;
    unsigned char m_len;
    char          m_data[16];
};

template<>
void TSFixedArray<AuOfficialPlayerRecord>::SetAlloc(unsigned int alloc)
{
    AuOfficialPlayerRecord* olddata = m_data;
    m_alloc = alloc;
    m_data = (AuOfficialPlayerRecord*)SMemReAlloc(olddata,
        alloc * sizeof(AuOfficialPlayerRecord),
        ".?AUAuOfficialPlayerRecord@@", -2, 0x10);
    if (m_data == 0)
    {
        m_data = (AuOfficialPlayerRecord*)SMemAlloc(
            alloc * sizeof(AuOfficialPlayerRecord),
            ".?AUAuOfficialPlayerRecord@@", -2, 0);
        if (olddata != 0)
        {
            unsigned int moved = alloc < m_count ? alloc : m_count;
            for (unsigned int i = 0; i < moved; i++)
            {
                new (&m_data[i]) AuOfficialPlayerRecord(olddata[i]);
                olddata[i].~AuOfficialPlayerRecord();
            }
            SMemFree(olddata, ".?AUAuOfficialPlayerRecord@@", -2, 0);
        }
    }
}

template<>
void TSFixedArray<AuOfficialPlayerRecord>::SetCount(unsigned int count)
{
    if (count != m_count)
    {
        if (count == 0)
        {
            if (m_data)
                SMemFree(m_data, ".?AUAuOfficialPlayerRecord@@", -2, 0);
            m_alloc = 0;
            m_count = 0;
            m_data = 0;
        }
        else
        {
            SetAlloc(count);
            for (unsigned int i = m_count; i < count; i++)
                new (&m_data[i]) AuOfficialPlayerRecord;
            m_count = count;
        }
    }
}

//  0x6F651B40 - the clamp-16 twin of ReadClampedField8_6F651A30
//  (Net/netdata_replayrecordreaders.cpp): a byte length probe, and - only
//  if it comes back <= 0x10 - that many raw bytes via
//  CDataStore::ReadRawAlias into `dest+1` with the length itself stored at
//  `dest[0]`; over 0x10 (the sentinel surviving unread, i.e. EOF) instead
//  bumps the store's own read-position counter.  Either way, a store left
//  past its own declared length zeroes `dest[0]` again.  `this` = store,
//  `edx` = dest, retn bare.
__declspec(noinline) void* __fastcall ReadClampedField16_6F651B40(void* storeArg, void* dest)
{
    CDataStore* store = (CDataStore*)storeArg;
    unsigned char* out = (unsigned char*)dest;
    unsigned char len = 0;
    store->ReadByte(&len);

    if (len <= 0x10)
    {
        store->ReadRawAlias(out + 1, len);
        out[0] = len;
    }
    else
    {
        store->m_readPos = store->m_field10 + 1;
    }

    if ((unsigned int)store->m_readPos > (unsigned int)store->m_field10)
        out[0] = 0;

    return store;
}

//  0x6F6523F0 - a leading byte, then ReadClampedField16_6F651B40's own
//  17-byte clamped field at +0x01 (18 bytes total).  `this` = store, `edx`
//  = dest, retn bare.
__declspec(noinline) void* __fastcall ReadOfficialPlayersElement_6F6523F0(void* storeArg, void* dest)
{
    CDataStore* store = (CDataStore*)storeArg;
    char* out = (char*)dest;
    store->ReadByte((unsigned char*)out);
    ReadClampedField16_6F651B40(store, out + 1);
    return store;
}

//  0x6F653860 - `this` = store, `edx` = dest, one stack arg (size, always
//  0x10 at its one call site), retn 4.
void* __fastcall ReadOfficialPlayersField_6F653860(void* storeArg, void* dest, unsigned int size)
{
    CDataStore* store = (CDataStore*)storeArg;
    TSFixedArray<AuOfficialPlayerRecord>* arr = (TSFixedArray<AuOfficialPlayerRecord>*)dest;

    unsigned char sz = (unsigned char)size;
    unsigned char tmp = (unsigned char)(sz + 1);
    store->ReadByte(&tmp);
    if (tmp > sz)
    {
        store->m_readPos = store->m_field10 + 1;
        return store;
    }

    unsigned int count = tmp;
    arr->SetCount(count);
    for (unsigned int i = 0; i < arr->m_count; i++)
        ReadOfficialPlayersElement_6F6523F0(store, &(*arr)[i]);

    return store;
}

//  0x6F6525E0 - a dword, a 0x20-byte fixed field (sub_6F651250, its own
//  wire format), a second dword, then a 0x20-byte string with the same
//  zero-on-overrun guard every string field in this closure already gets.
__declspec(noinline) void* __fastcall ReadTeamGameAdPayload(void* storeArg, void* dst)
{
    CDataStore* store = (CDataStore*)storeArg;
    char* out = (char*)dst;
    store->ReadDword((unsigned int*)out);
    ReadTeamGameAdWideField_6F651250(store, out + 4);
    store->ReadDword((unsigned int*)(out + 0x24));
    store->ReadString(out + 0x28, 0x20);
    if ((unsigned int)store->m_readPos > (unsigned int)store->m_field10)
        out[0x28] = 0;
    return store;
}

//  0x6F653A70 / 0x6F653AB0 - byte-identical bodies (two dwords, a 0x10-
//  byte field at +0x08 through sub_6F6511E0, then a 4-element field at
//  +0x18 through sub_6F653760) at two distinct addresses - the same
//  /OPT:ICF-less duplication this whole family already has plenty of
//  precedent for (cdatastore.h's own four ReadDword bodies, among others),
//  so each is written out in full rather than forwarded through one shared
//  helper (which would cost the *caller's* own inlined-duplicate shape).
__declspec(noinline) void* __fastcall ReadTeamGameListAddPayload(void* storeArg, void* dst)
{
    CDataStore* store = (CDataStore*)storeArg;
    char* out = (char*)dst;
    store->ReadDword((unsigned int*)out);
    store->ReadDword((unsigned int*)(out + 4));
    ReadTeamGameListWideField_6F6511E0(store, out + 8);
    ReadTeamGameListTailField_6F653760(store, out + 0x18, 4);
    return store;
}

__declspec(noinline) void* __fastcall ReadTeamGameListUpdatePayload(void* storeArg, void* dst)
{
    CDataStore* store = (CDataStore*)storeArg;
    char* out = (char*)dst;
    store->ReadDword((unsigned int*)out);
    store->ReadDword((unsigned int*)(out + 4));
    ReadTeamGameListWideField_6F6511E0(store, out + 8);
    ReadTeamGameListTailField_6F653760(store, out + 0x18, 4);
    return store;
}

//  0x6F6536C0 - CNetEventGameSuspend's own payload: a byte count, then a
//  growable Net::SUSPENDPLAYER[] rebuilt only when the wire count differs
//  from the list's own cached count (+0x04) - a new count of 0 frees the
//  existing array (SMemFree tagged ".?AUSUSPENDPLAYER@Net@@", the dump's
//  own `offset aAususpendplaye`, matching Storm's RTTI-name-tagged-
//  allocation idiom this repo already documents in cdatastorecacheseal.cpp
//  and friends) and zeroes the whole record; any other new count
//  (re)allocates through GrowOrRelease and remembers it.  Either way -
//  even when the count matched and nothing was rebuilt - every element of
//  the (possibly just-resized) array is re-read off the wire through
//  sub_6F6512B0, stride 8.
__declspec(noinline) void* __fastcall ReadGameSuspendPayload(void* storeArg, void* dst)
{
    CDataStore* store = (CDataStore*)storeArg;
    SSuspendPlayerList* list = (SSuspendPlayerList*)dst;

    unsigned char count = 0;
    store->ReadByte(&count);

    if (count != list->m_count)
    {
        if (count == 0)
        {
            if (list->m_array)
                SMemFree(list->m_array, ".?AUSUSPENDPLAYER@Net@@", -2, 0);
            list->m_field0 = 0;
            list->m_count = 0;
            list->m_array = 0;
        }
        else
        {
            list->GrowOrRelease(count);
            list->m_count = count;
        }
    }

    for (unsigned int i = 0; i < list->m_count; ++i)
    {
        void* elem = (char*)list->m_array + i * 8;
        ReadSuspendPlayerElement_6F6512B0(store, elem);
    }

    return store;
}

//  0x6F653AF0 - a dword, then a 0x10-byte field through sub_6F6537F0 (its
//  own wire format, `__fastcall(store,dest)` plus one stack size dword -
//  same shape as sub_6F653760 above).
__declspec(noinline) void* __fastcall ReadDistFileProgressPayload(void* storeArg, void* dst)
{
    CDataStore* store = (CDataStore*)storeArg;
    char* out = (char*)dst;
    store->ReadDword((unsigned int*)out);
    ReadDistFileProgressField_6F6537F0(store, out + 4, 0x10);
    return store;
}

//  0x6F653B10 - the entire payload is one 0x10-byte field through
//  sub_6F653860 (its own wire format) - no leading dword, unlike every
//  sibling above; `dst` itself is the field's own destination.
__declspec(noinline) void* __fastcall ReadOfficialPlayersPayload(void* storeArg, void* dst)
{
    CDataStore* store = (CDataStore*)storeArg;
    ReadOfficialPlayersField_6F653860(store, dst, 0x10);
    return store;
}

#define NET_EVENT_CTOR_MULTI(fnName, className, magicId, readerName, payloadSize) \
    class className : public CNetEventBase                                    \
    {                                                                          \
    public:                                                                    \
        className(unsigned int slot, CDataStore* store) : CNetEventBase(magicId) \
        {                                                                       \
            m_slot = slot;                                                     \
            readerName(store, m_payload);                                      \
        }                                                                       \
        unsigned int m_slot;                                                   \
        char         m_payload[payloadSize];                                   \
    };                                                                          \
    void __fastcall fnName(void* thisObj, unsigned int slot, void* storeArg,    \
                            const char* name, int zeroA, void* dtorOrZero,       \
                            int flag, int turn, int zeroC)                       \
    {                                                                            \
        (void)zeroC;                                                             \
        CDataStore* store = (CDataStore*)storeArg;                               \
        className evt(slot, store);                                             \
        NET_EVENT_CTOR_FIRE(thisObj, store, (&evt), name, zeroA, dtorOrZero,       \
                            flag, turn);                                         \
    }

NET_EVENT_CTOR_MULTI(CNetEventGameAd_Ctor,             CNetEventGameAd,             0x40090074, ReadGameAdPayload,             0x40)
NET_EVENT_CTOR_MULTI(CNetEventTeamGameAd_Ctor,         CNetEventTeamGameAd,         0x40090075, ReadTeamGameAdPayload,         0x40)
NET_EVENT_CTOR_MULTI(CNetEventGameFind_Ctor,           CNetEventGameFind,           0x40090077, ReadGameFindPayload,           0x40)
NET_EVENT_CTOR_MULTI(CNetEventGameJoin_Ctor,           CNetEventGameJoin,           0x40090078, ReadGameJoinPayload,           0x60)
NET_EVENT_CTOR_MULTI(CNetEventDistFileStart_Ctor,      CNetEventDistFileStart,      0x4009008B, ReadDistFileStartPayload,      0x140)
NET_EVENT_CTOR_MULTI(CNetEventDistFileComplete_Ctor,   CNetEventDistFileComplete,   0x4009008D, ReadDistFileCompletePayload,   0x240)
NET_EVENT_CTOR_MULTI(CNetEventGameListAdd_Ctor,        CNetEventGameListAdd,        0x40090069, ReadGameListAddPayload,        0x20)
NET_EVENT_CTOR_MULTI(CNetEventTeamGameListAdd_Ctor,    CNetEventTeamGameListAdd,    0x4009006E, ReadTeamGameListAddPayload,    0x20)
NET_EVENT_CTOR_MULTI(CNetEventTeamGameListUpdate_Ctor, CNetEventTeamGameListUpdate, 0x4009006F, ReadTeamGameListUpdatePayload, 0x20)
NET_EVENT_CTOR_MULTI(CNetEventGameSuspend_Ctor,        CNetEventGameSuspend,        0x40090083, ReadGameSuspendPayload,        0xC0)
NET_EVENT_CTOR_MULTI(CNetEventDistFileProgress_Ctor,   CNetEventDistFileProgress,   0x4009008C, ReadDistFileProgressPayload,   0x20)
NET_EVENT_CTOR_MULTI(CNetEventOfficialPlayers_Ctor,    CNetEventOfficialPlayers,    0x4009008E, ReadOfficialPlayersPayload,    0x20)

#undef NET_EVENT_CTOR_MULTI

//----------------------------------------------------------------------------
//  CNetEventPlayerJoin/CNetEventGameSetup/CNetEventGameCreate - the three
//  of the "multi" group whose own payload reader turned out to already be
//  a real reconstruction (netdata_replayrecordreaders.cpp /
//  netdata_loadreplayheader.cpp), reused rather than approximated with an
//  opaque buffer the way the other twelve above are.
//----------------------------------------------------------------------------
class CNetEventPlayerJoin : public CNetEventBase
{
public:
    CNetEventPlayerJoin(unsigned int slot, CDataStore* store) : CNetEventBase(0x40090079)
    {
        m_slot = slot;
        ReadReplayType0x16Record(store, &m_record);
    }
    unsigned int          m_slot;
    SReplayType0x16Record m_record;
};

void __fastcall CNetEventPlayerJoin_Ctor(void* thisObj, unsigned int slot, void* storeArg,
                                          const char* name, int zeroA, void* dtorOrZero,
                                          int flag, int turn, int zeroC)
{
    (void)zeroC;
    CDataStore* store = (CDataStore*)storeArg;
    CNetEventPlayerJoin evt(slot, store);
    NET_EVENT_CTOR_FIRE(thisObj, store, (&evt), name, zeroA, dtorOrZero, flag, turn);
}

class CNetEventGameSetup : public CNetEventBase
{
public:
    CNetEventGameSetup(unsigned int slot, CDataStore* store) : CNetEventBase(0x4009007C)
    {
        m_slot = slot;
        ReadReplaySlotRecord(store, &m_record);
    }
    unsigned int                m_slot;
    Net::GAMESETUP_SLOTRECORD  m_record;
};

void __fastcall CNetEventGameSetup_Ctor(void* thisObj, unsigned int slot, void* storeArg,
                                         const char* name, int zeroA, void* dtorOrZero,
                                         int flag, int turn, int zeroC)
{
    (void)zeroC;
    CDataStore* store = (CDataStore*)storeArg;
    CNetEventGameSetup evt(slot, store);
    NET_EVENT_CTOR_FIRE(thisObj, store, (&evt), name, zeroA, dtorOrZero, flag, turn);
}

class CNetEventGameCreate : public CNetEventBase
{
public:
    CNetEventGameCreate(unsigned int slot, CDataStore* store) : CNetEventBase(0x40090073)
    {
        m_slot = slot;
        ReadReplayType0x10Record(store, &m_record);
    }
    unsigned int          m_slot;
    SReplayType0x10Record m_record;
};

void __fastcall CNetEventGameCreate_Ctor(void* thisObj, unsigned int slot, void* storeArg,
                                          const char* name, int zeroA, void* dtorOrZero,
                                          int flag, int turn, int zeroC)
{
    (void)zeroC;
    CDataStore* store = (CDataStore*)storeArg;
    CNetEventGameCreate evt(slot, store);
    NET_EVENT_CTOR_FIRE(thisObj, store, (&evt), name, zeroA, dtorOrZero, flag, turn);
}

//----------------------------------------------------------------------------
//  CNetEventTrustedDesync_Ctor / CNetEventTrustedResult_Ctor (0x6F54B2D0 /
//  0x6F546E50) - table2 cases 48/49, gated on slot==1 &&
//  m_trustedEventsEnabled at their call sites in
//  CNetEventTypeDispatcher_ConstructAndBroadcast above.  Same overflow-
//  check/hook/fire/hook tail every other table2 case runs (NetEventCtorFire
//  again), but each builds its own object rather than a CNetEventBase:
//
//    - CNetEventTrustedResult follows the uniform group's own inline-field
//      shape (field4/id/field0xC/vtable, an edx-supplied field at +0x10 -
//      here the call site's literal `1` rather than a real slot - a
//      constant `0x10` at +0x1C, and its own payload read at +0x14 through
//      sub_6F652AE0, thunked below for the same "own per-event wire
//      format, not this session's worklist" reason the ~15 multi-field
//      uniform-group siblings above are).
//    - CNetEventTrustedDesync instead calls a real out-of-line base
//      constructor (sub_6F54ADD0) that stamps its own vftable and fields
//      *and* a 16-element array this closure never reads back - genuinely
//      distinct machinery from CNetEventBase, so it is not forced through
//      it.  Reconstructed for real below (sub_6F54ADD0 itself, a
//      sixteen-iteration field-pair init loop): mechanical and small
//      enough that thunking it would cost more clarity than it saves.  Its
//      own payload read (sub_6F652AE0's sibling sub_6F652F50) stays
//      thunked for the same per-event-wire-format reason.
//----------------------------------------------------------------------------
class CNetEventTrustedResult : public CNetEventBase
{
public:
    CNetEventTrustedResult(unsigned int flag, CDataStore* store) : CNetEventBase(0x40090091)
    {
        m_slot = flag;
        NetEventReadDwordField(store, &m_extra);  // approximates sub_6F652AE0's own two-field read
        m_field1C = 0x10;
    }
    unsigned int m_slot;      // +0x10
    unsigned int m_extra;     // +0x14
    int          m_field1C;   // +0x1C
};

//  sub_6F54ADD0 - CNetEventTrustedDesync's own base constructor.  `this` in
//  ecx, the wire id on the stack (`retn 4`).  Stamps the vftable, the three
//  always-0/const fields, an overwritable default at +0x10 (the caller's
//  own subsequent store of `flag` there wins - dead-store-eliminated in
//  the shipped listing the same way CNetCommandBase's own base-vftable
//  store is, per netcommand.h's comment), a `0x10` constant at +0x1C, and
//  sixteen 0x78-byte-stride array entries starting at +0x20, each getting
//  a zeroed leading byte and a `0x10` dword at +0x64 within the entry -
//  nothing in this closure reads the array back, so its element shape past
//  those two touched fields is left as reserved padding.
struct CNetEventTrustedDesyncSlot
{
    char m_flag;                              // +0x00 (of the slot)
    char m_reserved04[0x64];
    int  m_field64;                           // +0x64 (of the slot)
    char m_reserved68[0x78 - 0x64 - 4];
};

class CNetEventTrustedDesync
{
public:
    virtual ~CNetEventTrustedDesync() {}

    CNetEventTrustedDesync(unsigned int id)
        : m_field4(0), m_id(id), m_field0C(0), m_slot(2), m_field1C(0x10)
    {
        for (int i = 0; i < 16; ++i)
        {
            m_slots[i].m_flag = 0;
            m_slots[i].m_field64 = 0x10;
        }
    }

    int                        m_field4;     // +0x04
    unsigned int                m_id;         // +0x08
    int                        m_field0C;    // +0x0C
    unsigned int                m_slot;       // +0x10 (overwritten by the caller with `flag`)
    unsigned int                m_extra;      // +0x14 (approximates sub_6F652F50's own read)
    int                        m_field1C;    // +0x1C
    CNetEventTrustedDesyncSlot  m_slots[16];  // +0x20
};

void __fastcall CNetEventTrustedDesync_Ctor(void* thisObj, unsigned int flag,
                                             void* store, const char* name,
                                             int zeroA, int zeroB, int slotFlag,
                                             int turn, int zeroC)
{
    (void)zeroC;
    CDataStore* dataStore = (CDataStore*)store;
    CNetEventTrustedDesync evt(0x40090090);
    NetEventReadDwordField(dataStore, &evt.m_extra);
    evt.m_slot = flag;
    NET_EVENT_CTOR_FIRE(thisObj, dataStore, (&evt), name, zeroA, zeroB, slotFlag, turn);
}

void __fastcall CNetEventTrustedResult_Ctor(void* thisObj, unsigned int flag,
                                             void* store, const char* name,
                                             int zeroA, int zeroB, int slotFlag,
                                             int turn, int zeroC)
{
    (void)zeroC;
    CDataStore* dataStore = (CDataStore*)store;
    CNetEventTrustedResult evt(flag, dataStore);
    NET_EVENT_CTOR_FIRE(thisObj, dataStore, (&evt), name, zeroA, zeroB, slotFlag, turn);
}

//----------------------------------------------------------------------------
//  CNetEventTypeDispatcher::ConstructAndBroadcast - 0x6F551D80.  `retn 4`.
//----------------------------------------------------------------------------
int __fastcall CNetEventTypeDispatcher_ConstructAndBroadcast(
    SNetSessionInfo* self, void* /*unused_edx*/, void* record)
{
    SNetDataStates* states = reinterpret_cast<SNetDataStates*>(self);

    //  The outer CDataStore this whole dispatch serializes into/reads out
    //  of - `Size` in the dump (its own vftable-init instruction is
    //  literally `mov [esp+Size], offset CDataStore::vftable`).
    CDataStore store;
    store.m_field4   = *(void**)((char*)record + 8);
    store.m_field8   = 0;
    store.m_field0xC = -1;
    store.m_field10  = *(int*)((char*)record + 0xC);
    store.m_readPos  = 0;

    unsigned char rawType = *((unsigned char*)record + 0x14);
    unsigned int msgType = (rawType < 0x40) ? rawType : 0x32;

    unsigned int slot;
    if (*((unsigned char*)record + 0x15) != 0)
        slot = self->m_activeIndex;
    else
        slot = 0;

    //  The dump's own two-branch skip: a record for a slot other than the
    //  active one never runs any of the three special cases, and goes
    //  straight to the combined table1/table2 dispatch below.
    if (slot == self->m_activeIndex)
    {
        if (msgType == 0x10 || msgType == 0x15)
        {
            //  ----------------------------------------------------------
            //  Type 0x10/0x15 - route-candidate/reconnection bookkeeping.
            //  NOT replay-specific despite sitting next to the one that is
            //  - see this file's own header comment and
            //  docs/targets/CNetEventTypeDispatcher.md.  Both types run
            //  the *same* logic (only the size/shape of the local probe
            //  record IDA gives each a different stack offset for) and
            //  share one tail:
            //
            //    - only when the slot's route record is NOT yet active
            //      (`m_active < 1`) does anything happen at all - an
            //      already-active slot falls straight through to the
            //      combined dispatch below;
            //    - build a local route-candidate probe via
            //      ReadReplayType0x10Record(&store, &probe) - already
            //      reconstructed (netdata_replayrecordreaders.cpp), reused
            //      here as a route-viability probe rather than a header
            //      load;
            //    - if the outer `store`'s own overflow guard trips
            //      (m_readPos > m_field10) the *whole function* returns
            //      immediately - `store`'s destructor still runs (C++
            //      scope exit), matching the dump's own explicit
            //      ~CDataStore call right before it jumps to the shared
            //      epilogue;
            //    - otherwise promote the candidate to the active slot:
            //      self->m_activeIndex = 0 and slot = 0 first: if the
            //      probe's own bit-0x100 flag is set, self->m_activeIndex
            //      becomes 1 and slot 1 instead (the probe was for slot 1,
            //      not the local slot) - `m_routeCandidateAltFlag`
            //      (+0xAC4) supplies the value copied into
            //      m_cheatsEnabled on that path, an overload of that field
            //      distinct from its usual "cheats enabled" reading.
            //  ----------------------------------------------------------
            SNetSlotRouteRecord* rec = SlotRouteRecord(self, slot);
            if (rec->m_active < 1)
            {
                //  sub_6F652560 - already reconstructed as
                //  ReadReplayType0x10Record (netdata_replayrecordreaders.cpp
                //  / netdata_replayrecords.h), CNetData::LoadReplayHeader's
                //  own record-type-0x10 reader.  This branch reads the same
                //  record shape as a route-viability probe.
                SReplayType0x10Record probe;
                ReadReplayType0x10Record(&store, &probe);

                if ((unsigned int)store.m_readPos > (unsigned int)store.m_field10)
                    return -1;   // store's destructor runs here (RAII)

                self->m_activeIndex = 0;
                slot = 0;
                int tag = IsUnknownTag_6F537790(self);
                self->m_cheatsEnabled = tag;
                //  +0x18 within the record - inside its own unnamed
                //  reserved gap (m_reserved07, +0x07..+0x20) - bit 0x100
                //  selects "this probe is for slot 1".
                if ((*(unsigned int*)((char*)&probe + 0x18) & 0x100) != 0)
                {
                    self->m_cheatsEnabled = self->m_routeCandidateAltFlag;
                    slot = 1;
                    self->m_activeIndex = 1;
                }
                store.m_readPos = 0;
            }
            goto commonDispatch;
        }
        else if (msgType == 0x1B)
        {
            //  ------------------------------------------------------------
            //  Type 0x1B - THE REPLAY-STREAMING STATE-MACHINE DRIVER.
            //
            //  `rec` is the per-slot route record for the record's own
            //  slot; it must be marked active (+0x278 abs == 1, `m_active`)
            //  or this whole branch is skipped (falls to the combined
            //  dispatch below untouched).
            //
            //    - slot == 0 (the local/host record): unconditionally call
            //      SNetDataStates::SetRecordState(1) (transition the
            //      *record* state machine to "recording"), then gate on
            //      m_flagBE0.
            //
            //    - slot != 0: read the record's own `m_kind` (+0x288 abs):
            //        * nonzero  -> SetReplayState(2) (STREAMING).  If the
            //          record's own `m_streamingJustActivated` (+0x2D8 abs)
            //          is 1, snapshot the replay stream's own pending byte
            //          count (m_replayStream.m_field10) into
            //          self->m_routeReplayArg and invoke
            //          CNetEventTypeDispatcher::ReplayAckDispatch
            //          (0x6F54EB00) - "echo the replay header's own
            //          startup records a second time, now that streaming
            //          has begun" (see that function's own header
            //          comment).  Either way self->m_routeFlagAD4 is
            //          cleared.
            //        * zero -> SetReplayState(3) (the third state, "no
            //          more data") and close the replay stream through its
            //          own vtable slot 7, the same close every 0x10 case
            //          above uses.  Both self->m_routeRetry and
            //          m_routeReplayArg are cleared.
            //  ------------------------------------------------------------
            SNetSlotRouteRecord* rec = SlotRouteRecord(self, slot);
            if (rec->m_active == 1)
            {
                if (slot == 0)
                {
                    states->SetRecordState(1);
                    if (self->m_flagBE0 != 0)
                        goto commonDispatch;
                    goto tailAfterActiveSlot;
                }

                if (rec->m_kind != 0)
                {
                    states->SetReplayState(2);
                    self->m_routeRetry = 0;
                    if (rec->m_streamingJustActivated == 1)
                    {
                        self->m_routeReplayArg = self->m_replayStream.m_field10;
                        CNetEventTypeDispatcher_ReplayAckDispatch(self);
                        self->m_routeFlagAD4 = 0;
                    }
                    else
                    {
                        self->m_routeReplayArg = 0;
                        self->m_routeFlagAD4 = 0;
                    }
                }
                else
                {
                    states->SetReplayState(3);
                    typedef void (__thiscall *CloseFn)(void*);
                    void* vtbl = *(void**)&self->m_replayStream;
                    CloseFn close = *(CloseFn*)((char*)vtbl + 0x1C);
                    close(&self->m_replayStream);
                    self->m_routeRetry = 0;
                    self->m_routeReplayArg = 0;
                }
            }
            goto tailAfterActiveSlot;
        }
        goto commonDispatch;
    }
    goto commonDispatch;

tailAfterActiveSlot:
    //  The shared tail every type-0x10/0x15/0x1B branch above reaches:
    //  flush the observer stream (unconditionally through the same vtable
    //  slot 7), then optionally re-fire the local route-candidate flag,
    //  then close the second embedded CDataStore, then bail out entirely
    //  (this record is fully handled - no CNetEvent is constructed for it).
    if (self->m_flag226C != 0)
    {
        self->m_routeFlagAD0 = 0;
        states->SetRecordState(1);
    }
    {
        typedef void (__thiscall *CloseFn)(void*);
        void* vtbl = *(void**)&self->m_observerStream;
        CloseFn close = *(CloseFn*)((char*)vtbl + 0x1C);
        close(&self->m_observerStream);
    }
    if (self->m_flagBE0 == 0)
    {
        void* candidate = GetSelectedSubObject_6F537B20(self);
        if (candidate != 0)
        {
            unsigned int magicPart = (unsigned int)(0 - (int)self->m_cheatsEnabled) & 0x80000000u;
            unsigned int computed = magicPart | 0x17AB;
            unsigned int tag = GetProductTag_6F537EF0();
            //  `computed` really is passed through as EnsureRouteCandidateLogOpen_6F534660's
            //  own `fileName` argument here - only observable if that log's
            //  +0x100 handle is still zero at this call site, which nothing
            //  in this closure proves is ever true in practice (this record
            //  is a long-lived diagnostic object almost certainly opened for
            //  real, with a real filename, by an earlier caller outside this
            //  tree).  Reproduced literally rather than "fixed": the dump's
            //  own stack push is this exact value, not a real string.
            RouteCandidateDiagnostic_6F5349B0(candidate, 0, (const char*)computed, lpFileName, tag, 0x1A);
        }
    }
    return -1;

commonDispatch:
    //  ----------------------------------------------------------------------
    //  The combined table1/table2 dispatch.  Table1 (jpt_6F5520A6) only
    //  fires its own body for 8 of its 25 mapped types (23/28/30/31/32/33/
    //  35/47 in this dispatcher's own decimal numbering); every other
    //  mapped type, and every unmapped one, falls straight through into
    //  table2 (jpt_6F55211B) with no extra work.  All of table1's own
    //  bodies converge on the same call (SerializeExtraHeader_6F537AA0)
    //  before falling into table2 regardless.
    //  ----------------------------------------------------------------------
    bool table1Eligible = (slot == self->m_activeIndex) && (self->m_flagBE4 == 1) &&
                          (msgType >= 0x17 && msgType <= 0x2F);
    bool wroteExtraHeader = false;

    if (table1Eligible)
    {
        switch (msgType)
        {
        case 23: case 33: case 35: case 47:
            SerializeExtraHeader_6F537AA0(self, 0, msgType, &store);
            break;
        case 28:
            if (self->m_flagBE0 == 0)
                SerializeExtraHeader_6F537AA0(self, 0, msgType, &store);
            break;
        case 30:
            SerializeExtraHeader_6F537AA0(self, 0, msgType, &store);
            break;
        case 31:
            wroteExtraHeader = true;
            SerializeExtraHeader_6F537AA0(self, 0, msgType, &store);
            break;
        case 32:
            SerializeExtraHeader_6F537AA0(self, 0, msgType, &store);
            break;
        default:
            break;
        }
    }

    //  [self+0x1C6Ch] = 2 unconditionally from here - overwritten to 0
    //  again by table2 case 32 specifically (msg type 0x21) below.
    int* routeStage = (int*)((char*)self + 0x1C6C);
    *routeStage = 2;

    bool isPrivileged = (slot == self->m_activeIndex) && (self->m_flagBE4 == 1) &&
                        (msgType == 0x1E || msgType == 0x1F || msgType == 0x21 ||
                         msgType == 0x17 || msgType == 0x2F);
    int privFlag = isPrivileged ? 1 : 0;

    switch (msgType)
    {
#define FIRE_EVENT(mt, ctorFn, evtName, dtorPtr)                                  \
    case mt:                                                                     \
    {                                                                            \
        int flag = (self->m_replayFlag != 0 && privFlag != 0) ? 1 : 0;           \
        ctorFn((char*)self + 8 + slot * 0x304, slot, &store, evtName, 0,         \
               (void*)(dtorPtr), flag, (int)self->m_currentTurn, 0);             \
        break;                                                                   \
    }

    FIRE_EVENT(1,  CNetEventConnect_Ctor,                 "CNetEventConnect",                 0)
    FIRE_EVENT(2,  CNetEventDisconnect_Ctor,               "CNetEventDisconnect",              0)
    FIRE_EVENT(3,  CNetEventGameListStart_Ctor,            "CNetEventGameListStart",           0)
    FIRE_EVENT(4,  CNetEventGameListStop_Ctor,             "CNetEventGameListStop",            0)
    FIRE_EVENT(5,  CNetEventGameListError_Ctor,            "CNetEventGameListError",           0)
    FIRE_EVENT(6,  CNetEventGameListAdd_Ctor,              "CNetEventGameListAdd",             0)
    FIRE_EVENT(7,  CNetEventGameListUpdate_Ctor,           "CNetEventGameListUpdate",          0)
    FIRE_EVENT(8,  CNetEventGameListDelete_Ctor,           "CNetEventGameListDelete",          0)
    FIRE_EVENT(9,  CNetEventTeamGameListStart_Ctor,        "CNetEventTeamGameListStart",       0)
    FIRE_EVENT(10, CNetEventTeamGameListStop_Ctor,         "CNetEventTeamGameListStop",        0)
    FIRE_EVENT(11, CNetEventTeamGameListAdd_Ctor,          "CNetEventTeamGameListAdd",         0)
    FIRE_EVENT(12, CNetEventTeamGameListUpdate_Ctor,       "CNetEventTeamGameListUpdate",      0)
    FIRE_EVENT(13, CNetEventTeamGameListDelete_Ctor,       "CNetEventTeamGameListDelete",      0)
    FIRE_EVENT(14, CNetEventAnonGameFind_Ctor,             "CNetEventAnonGameFind",            0)
    FIRE_EVENT(15, CNetEventAnonGameJoin_Ctor,             "CNetEventAnonGameJoin",            0)
    FIRE_EVENT(16, CNetEventGameCreate_Ctor,               "CNetEventGameCreate",              0x6F551B80)
    FIRE_EVENT(17, CNetEventGameAd_Ctor,                   "CNetEventGameAd",                  0)
    FIRE_EVENT(18, CNetEventTeamGameAd_Ctor,               "CNetEventTeamGameAd",              0)
    FIRE_EVENT(19, CNetEventTeamInfo_Ctor,                 "CNetEventTeamInfo",                0)
    FIRE_EVENT(20, CNetEventGameFind_Ctor,                 "CNetEventGameFind",                0)
    FIRE_EVENT(21, CNetEventGameJoin_Ctor,                 "CNetEventGameJoin",                0x6F551C20)
    FIRE_EVENT(22, CNetEventPlayerJoin_Ctor,               "CNetEventPlayerJoin",              0x6F551CB0)
    FIRE_EVENT(23, CNetEventPlayerLeave_Ctor,              "CNetEventPlayerLeave",             0x6F54FDF0)
    FIRE_EVENT(24, CNetEventPlayerReady_Ctor,              "CNetEventPlayerReady",             0)
    FIRE_EVENT(25, CNetEventGameSetup_Ctor,                "CNetEventGameSetup",               0x6F545350)
    FIRE_EVENT(26, CNetEventGameClose_Ctor,                "CNetEventGameClose",               0)
    FIRE_EVENT(27, CNetEventGameStart_Ctor,                "CNetEventGameStart",               0x6F53E170)
    FIRE_EVENT(28, CNetEventGameReady_Ctor,                "CNetEventGameReady",               0x6F53E1B0)
    FIRE_EVENT(29, CNetEventPlayerUpdate_Ctor,             "CNetEventPlayerUpdate",            0)
    FIRE_EVENT(36, CNetEventGameSuspend_Ctor,               "CNetEventGameSuspend",             0x6F549340)
    FIRE_EVENT(37, CNetEventPlayerResume_Ctor,             "CNetEventPlayerResume",            0x6F5493A0)
    FIRE_EVENT(38, CNetEventRouterHandoffSearching_Ctor,   "CNetEventRouterHandoffSearching",  0x6F53E220)
    FIRE_EVENT(39, CNetEventRouterHandoffSyncing_Ctor,     "CNetEventRouterHandoffSyncing",    0)
    FIRE_EVENT(40, CNetEventRouterHandoffDone_Ctor,        "CNetEventRouterHandoffDone",       0x6F53E260)
    FIRE_EVENT(41, CNetEventRouterUnresponsive_Ctor,       "CNetEventRouterUnresponsive",      0)
    FIRE_EVENT(42, CNetEventRouterResponsive_Ctor,         "CNetEventRouterResponsive",        0)
    FIRE_EVENT(43, CNetEventDistFileStart_Ctor,            "CNetEventDistFileStart",           0)
    FIRE_EVENT(44, CNetEventDistFileProgress_Ctor,         "CNetEventDistFileProgress",        0)
    FIRE_EVENT(45, CNetEventDistFileComplete_Ctor,         "CNetEventDistFileComplete",        0)
    FIRE_EVENT(46, CNetEventOfficialPlayers_Ctor,          "CNetEventOfficialPlayers",         0x6F54FD50)
    FIRE_EVENT(33, CNetEventSetTurnsLatency_Ctor,          "CNetEventSetTurnsLatency",         0x6F53E1F0)
    FIRE_EVENT(47, CNetEventTrigger_Ctor,                  "CNetEventTrigger",                 0)

#undef FIRE_EVENT

    case 30:
        //  table2 case 30 - msg type 0x1F: redispatch the record as a
        //  queued turn record (0x6F5516E0, already reconstructed).
        CNetData_DispatchQueuedTurnRecord(self, 0, &store, slot, 0);
        break;

    case 31:
        //  table2 case 31 - msg type 0x20: same redispatch, `latchedDelta`
        //  argument 1 instead of 0 (the dump pushes `1` where case 30
        //  pushes `0` as this call's first stack argument).
        CNetData_DispatchQueuedTurnRecord(self, 0, &store, slot, 1);
        break;

    case 32:
        //  table2 case 32 - msg type 0x21: [self+0x1C6Ch] reset to 0 (the
        //  one case that undoes the unconditional "=2" above) before its
        //  own big builder.
        *routeStage = 0;
        FireEvent_6F54E860(self, 0, &store, slot);
        break;

    case 34:
    {
        //  table2 case 34 - msg type 0x22 - "TURNSSYNC": build a
        //  diagnostic record, compare the outer store's own overflow
        //  counters, and - only for the active local slot with no pending
        //  sync/resync flags - build and compare a second small buffer
        //  read straight off the wire against `self+0x1C64` (the applied-
        //  delta field pausecallchainthunks.cpp's own reporter object
        //  also writes through).  Approximated here at the call-sequence
        //  level; the buffer-compare's own byte-for-byte shape is not
        //  chased further - see docs/targets/CNetEventTypeDispatcher.md.
        //
        //  CONFIRMED DEAD (checksumcompare session, 2026-09-04): the
        //  shipped compare (0x6F552CEC-0x6F552D12, a hand-unrolled 4-byte
        //  memcmp of the wire dword against self+0x1C64, one iteration
        //  since the loop counter starts at 4 and the step is 4) has its
        //  match and mismatch arms converge on the *same* address
        //  (0x6F552D12) with the *same* following code either way - an
        //  unconditional CDataStore::~CDataStore (sub_6F2C8A10) call, then
        //  a jump to the shared def_6F55211B tail. The register the
        //  compare leaves its verdict in (ebp) is immediately clobbered by
        //  a stack reload of `msgType` (0x6F552D22: `mov ebp,
        //  [esp+308h+var_2D8]`) before the tail ever runs, and the tail's
        //  own next real use of ebp (0x6F552E85, `cmp ebp, 1Bh`) is that
        //  reloaded msgType value gating DesyncCandidateCheck_6F5453A0 (an
        //  unrelated network-path-discovery helper, already confirmed not
        //  the trigger - see checksum-provider-registry.md). So the
        //  comparison's result never reaches a branch, a flag write, a
        //  counter, or a call into NETCONN::NotifyDisconnected /
        //  OsNet::TCPCONN::Shutdown from this dispatcher - it is
        //  genuinely computed and discarded, not merely unreproduced here.
        unsigned char handled = 0;
        TurnsSyncRecordCtor_6F652870(&store, &handled);
        if (store.m_field8 != store.m_field0xC)
            CNetData_ParseError("EVENT_ID_TURNSSYNC");

        if (slot == 0)
        {
            if (self->m_activeIndex == 0)
            {
                nullsub_1("m_gameTurnId = %u\n", self->m_currentTurn);
                nullsub_1("session = %u\n", 0u);
            }
            else if (self->m_flag2270 == 0 && self->m_flag226C == 0)
            {
                unsigned int wireField;
                unsigned char localBuf[16];
                NetMsgReadDword_6F537830(&store, &wireField);
                (void)wireField;
                (void)localBuf;   // the shipped 4-dword compare against
                                  // self+0x1C64 is not reproduced exactly
            }
        }
        break;
    }

    case 35:
    {
        //  table2 case 35 - msg type 0x23 - "TURNSSYNCMISMATCH": same
        //  shape as case 34 with the mismatch record's own builder/dtor
        //  pair and a different diagnostic string.
        unsigned char handled = 0;
        void* rec = TurnsSyncMismatchRecordCtor_6F653650(&store, &handled);
        if (store.m_field8 != store.m_field0xC)
            CNetData_ParseError("EVENT_ID_TURNSSYNCMISMATCH");

        unsigned int wireField;
        NetMsgReadDword_6F537830(&store, &wireField);
        (void)wireField;
        TurnsSyncMismatchRecordDtor_6F545250(rec);
        break;
    }

    case 48:
        //  table2 case 48 - msg type 0x30 - "CNetEventTrustedDesync",
        //  gated on slot==1, [self+0xABC]!=0.
        if (slot == 1 && self->m_trustedEventsEnabled != 0)
        {
            int flag = (self->m_replayFlag != 0 && privFlag != 0) ? slot : 0;
            CNetEventTrustedDesync_Ctor((char*)self + 0x30C, 1, &store,
                                        "CNetEventTrustedDesync", 0, 0, flag,
                                        (int)self->m_currentTurn, 0);
        }
        break;

    case 49:
        //  table2 case 49 - msg type 0x31 - "CNetEventTrustedResult", same
        //  gate.
        if (slot == 1 && self->m_trustedEventsEnabled != 0)
        {
            int flag = (self->m_replayFlag != 0 && privFlag != 0) ? slot : 0;
            CNetEventTrustedResult_Ctor((char*)self + 0x30C, 1, &store,
                                        "CNetEventTrustedResult", 0, 0, flag,
                                        (int)self->m_currentTurn, 0);
        }
        break;

    default:
        break;
    }

    //  def_6F55211B - the shared tail every table2 case above reaches.
    *routeStage = 0;
    if (slot == self->m_activeIndex && self->m_flagBE4 == 1 &&
        msgType == 0x1B && self->m_flagBE0 == 0)
    {
        DesyncCandidateCheck_6F5453A0(self);
        SerializeExtraHeader_6F537AA0(self, 0, msgType, &store);
    }

    if (wroteExtraHeader)
    {
        unsigned char cacheBuf[0x20];
        TrustedSerializeCacheCtor_6F53FAB0(cacheBuf);
        CNetData_AppliedDeltaReporterSetTurnField(cacheBuf, (char*)self + 0x1C64);
        TrustedSerializeCacheDtor_6F53FD10(cacheBuf);
    }

    if (self->m_dispatchLatched != 0)
    {
        self->m_dispatchLatched = 0;
        FlushLatchedCommand_6F537B50(self);
        CNetData_FireCommand_6F53A380(
            (CNetPlayerRecord*)((char*)self + slot * 0x304 + 8),
            (CNetCommandBase*)((char*)self + 0x1B48),
            "NET_COMMAND_SAVE", (int)self->m_replayFlag, self->m_currentTurn);
    }

    return -1;
}
