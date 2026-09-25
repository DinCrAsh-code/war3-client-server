//============================================================================
//  SNetSessionInfo::SNetSessionInfo() / ::~SNetSessionInfo() - 0x6F550460 /
//  0x6F54FC50, the real `CNetData::CNetData()` / `::~CNetData()` per RTTI
//  (`agent_worktrees/classes/0x6F9590D0.json`) - cnd-ctor-agent-2 session,
//  finishing what the cnd-ctor-agent session left `TODO`
//  (docs/notes/checksum-provider-registry.md, "cnd-ctor-agent session").
//
//  Named `SNetSessionInfo::SNetSessionInfo`/`::~SNetSessionInfo` rather
//  than `CNetData::CNetData`/`::~CNetData` because `class CNetData` in this
//  same file is already a different, non-polymorphic type - the shipped
//  binary's own `CNetData::CNetSession` element struct this repo settled
//  on calling `CNetPlayerRecord` (see that class's own file header) - and
//  `SNetSessionInfo` is already this repo's established name for the real
//  array-base singleton's own view.  `tools/funcmap.py`'s readable name is
//  `CNetData::CNetData`/`CNetData::~CNetData` regardless, matching the real
//  shipped/RTTI identity; only the C++ spelling differs, the same way this
//  repo already spells `CSyncData` without its real `NTempest::` qualifier
//  (syncdata.h) or spells the shipped `CNetData::CNetSession` as
//  `CNetPlayerRecord`.
//
//  Both are `__CxxFrameHandler3`-SEH-framed (docs/msvc-vc8-idioms.md, "An
//  `__except_handler4`-shaped frame this toolchain cannot reproduce") -
//  the score ceiling that caps this pair is expected and does not by
//  itself mean anything is wrong; the verdict is DIFFERS-pending-angr, not
//  EXACT, for that reason alone even before any other gap.
//
//  A few fields this constructor writes have no established purpose beyond
//  "the constructor sets it to this constant" and are written through raw
//  offsets rather than added as named struct members, per CLAUDE.md's own
//  "only touched members get names" rule for a field nothing else in this
//  closure reads.  Two (the +0x22AC/+0x22B0 float pair) are a genuine,
//  flagged gap: the real computation reads two globals
//  (`dword_6FA89158`, an unidentified int; `dbl_6F957640`, an unidentified
//  double divisor; `flt_6F875AB8`, a conditional normalize constant) this
//  session did not identify or register in `tools/funcmap.py`'s `DATA`
//  table, so they are approximated as a literal `0.0f` rather than guessed
//  at - a concrete, named DIFFERS gap for a follow-up session rather than
//  a silent wrong value.
//============================================================================
#include "netdata.h"
#include "netobserver.h"
#include "cdatarecycler.h"
#include "gamedatawriter.h"   // SGameDataBase::Construct, 0x6F534CE0
#include "storm.h"
#include <string.h>           // memset

//  0x6F535DC0 - Game/gamerestartthunks.cpp's own naked redirect, reused
//  here for both embedded CDataStore-shaped members' teardown (matches
//  the real shipped call target exactly - same address, same signature).
void __fastcall SaveGameBaseDestruct(void* self);

//  0x6F2C9290 / 0x6F2C95B0 - already real reconstructions under these
//  names in netcommand_dispatch.cpp (CDataStoreCache1460's own base
//  construct/destroy, reached there from a completely different closure -
//  CLAUDE.md's own "grep src/ for its address" rule: a hit means call the
//  reconstruction that is already there rather than declaring a second
//  naked redirect to the same two addresses).
void __fastcall CDataStoreCache1460_ConstructInPlace(void* self);
void __fastcall CDataStoreCache1460_DestroyInPlace(void* self);

//  0x6F54B000 / 0x6F54B040 - Net/netdataeventqueueheader.cpp;
//  0x6F54B850 - Net/netdataeventqueuedrain.cpp.
void __fastcall ConstructNetDataEventQueueHeader(void* self);
void __fastcall DestroyNetDataEventQueueHeader(void* self);
void __fastcall DrainNetDataEventQueue(void* self);

const void* const kCNetDataVftable          = (const void*)0x6F9590D0;
const void* const kCNetCommandSaveVftable   = (const void*)0x6F9509B4;
const void* const kInstanceRecyclerVftable  = (const void*)0x6F95851C;
const void* const kNetTurnStoreVftable      = (const void*)0x6F958670;
const void* const kNTempestCEntityVftable   = (const void*)0x6F951784;
const void* const kTRefCntVftable           = (const void*)0x6F8765F8;

#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

//  0x6F652040 - Net::CTurnStore's own extra init call: byte-for-byte the
//  same "reserve room for one more record" shape as the already-real
//  Net::CTurnsSyncStore::ReserveRecord (Net/netturnsyncstore.cpp,
//  0x6F652080, EXACT) - same CDataStoreCache1460-based field layout
//  (m_field4/+4, m_field8/+8, m_field0xC/+0xC, m_count/+0x10), same
//  two-part guard shape, same vtable slot 3 (Grow, sub_6F2C9450 - shared
//  with CTurnsSyncStore per docs/targets/vtables/CTurnStore.txt) reached
//  indirectly rather than declared as a `virtual` (CLAUDE.md/
//  ctor_vtable_audit.py: only a class whose whole vtable is reconstructed
//  may have its constructor stamp our own table, and 9 of 11 slots here
//  are still real shipped addresses) - just an 8-byte record instead of a
//  5-byte one. Operates on the raw +0x1C78 sub-object the same way the
//  rest of this constructor does; nothing else in this closure needs a
//  named `Net::CTurnStore` class of its own.
typedef int (__thiscall *CTurnStoreGrowFn)(void* self, int count, int increment,
                                            void** pField4, int* pField8,
                                            int* pField0xC, int flagA, int flagB);

void __fastcall NetTurnStoreExtraInit(void* self)
{
    char* p = (char*)self;
    unsigned int count  = *(unsigned int*)(p + 0x10);
    unsigned int field8 = *(unsigned int*)(p + 0x8);

    if (count >= field8)
    {
        unsigned int field0xC = *(unsigned int*)(p + 0xC);
        if (count + 8 <= field0xC + field8)
            goto haveRoom;
    }

    ((CTurnStoreGrowFn)(*(void***)self)[0xC / 4])(
        self, count, 8, (void**)(p + 4), (int*)(p + 8), (int*)(p + 0xC), 0, 0);

haveRoom:
    *(unsigned int*)(p + 0x10) += 8;
}

//  0x6F6562A0 - a lazy-singleton critical-section acquire/register call
//  with no arguments and no receiver; reached only from this constructor's
//  own tail in this closure, so it is left as the one truly free-standing
//  thunk here rather than chased into Sync/'s own critical-section
//  machinery.
ADDR_THUNK(void RegisterNetDataSingleton(), 0x6F6562A0)

//  0x6F6562E0 - nullsub_68, a bare `retn` in the shipped stream; called for
//  real (not folded away) so the destructor's own instruction count still
//  accounts for it.
ADDR_THUNK(void NullSub68(), 0x6F6562E0)

SNetSessionInfo::SNetSessionInfo()
{
    char* p = (char*)this;

    *(void**)(p + 0) = (void*)kCNetDataVftable;

    //  m_records[2] (+0x08..+0x610) default-constructs automatically here -
    //  a real C++ array of a class with a real ctor/dtor reproduces the
    //  shipped `eh vector constructor iterator` call exactly (netdata.h's
    //  own field comment).  This scalar sits right next to it.
    *(int*)(p + 4) = 2;

    //  Two embedded CDataStore-shaped members, constructed through
    //  SGameDataBase::Construct(capacity=0x2000, flags=1) - the exact
    //  shape this ctor's own real call site uses (Game/gamedatawriter.h);
    //  cast rather than retyped, so netsessionend.cpp's own already-real
    //  `EndSessionGame`/CNetSendQueue usage is untouched.
    *(int*)(p + 0x614) = 0;
    ((SGameDataBase*)&m_replayStream)->Construct(0x2000, 1);

    *(unsigned char*)(p + 0x96C) = 0;
    *(unsigned char*)(p + 0x98C) = 0;
    *(unsigned char*)(p + 0x99C) = 0;
    *(unsigned short*)(p + 0xA24) = 0;
    *(int*)(p + 0xAA8) = 0;
    *(int*)(p + 0xAAC) = 0;
    *(void**)(p + 0xAB0) = 0;   //  freed in the destructor if set - tagged
                                //  ".?AUPLAYERDATA@Net@@"
    *(int*)(p + 0xAB4) = 0;

    *(int*)(p + 0xAC0) = 0;
    m_routeCandidateAltFlag = 0;   // +0xAC4
    m_routeRetry             = 0;  // +0xAC8
    m_routeReplayArg         = 0;  // +0xACC
    m_routeFlagAD0           = 0;  // +0xAD0
    m_routeFlagAD4           = 0;  // +0xAD4
    *(int*)(p + 0xBDC) = 0;
    m_flagBE0 = 0;                 // +0xBE0
    m_flagBE4 = 0;                 // +0xBE4
    ((SGameDataBase*)&m_observerStream)->Construct(0x2000, 1);

    *(int*)(p + 0xF30) = 0;
    *(int*)(p + 0xF34) = 0;
    ConstructNetDataEventQueueHeader(p + 0xF38);

    *(int*)(p + 0x173C) = 0;
    *(unsigned int*)(p + 0x1740) = 0xFF;

    *(int*)(p + 0x1B40) = 0;
    m_dispatchLatched = 0;         // +0x1B44
    *(int*)(p + 0x1B4C) = 0;
    *(unsigned int*)(p + 0x1B50) = 0xA0006;
    *(int*)(p + 0x1B54) = 0;
    m_dispatchLatchedPlayerIndex = 2;   // +0x1B58
    *(unsigned char*)(p + 0x1B5C) = 6;
    m_dispatchLatchedSender = 0;   // +0x1B5D
    //  CNetCommandSave - vtable-only construction (its own base's
    //  constructor call is fully inlined away here, the same "trivial base
    //  ctor collapses to nothing but the derived vtable stamp" idiom
    //  Game/gamewar3.cpp's own CSynchronousData note already documents).
    *(void**)(p + 0x1B48) = (void*)kCNetCommandSaveVftable;
    *(unsigned char*)(p + 0x1B60) = 0;

    m_currentTurn = 0;             // +0x1C68
    *(int*)(p + 0x1C6C) = 0;
    *(int*)(p + 0x1C70) = 0;
    m_cheatsEnabled = 0;           // +0x1C74
    //  Net::CTurnStore (+0x1C78) - CDataStoreCache1460's own base
    //  construction (already a real reconstruction elsewhere - see the
    //  extern declarations above), then its own derived vtable stamp and
    //  one further init call this session left a thunk.
    CDataStoreCache1460_ConstructInPlace(p + 0x1C78);
    *(void**)(p + 0x1C78) = (void*)kNetTurnStoreVftable;
    NetTurnStoreExtraInit(p + 0x1C78);

    *(int*)(p + 0x2248) = 1;
    *(int*)(p + 0x224C) = 0;
    *(int*)(p + 0x2250) = 0;
    *(int*)(p + 0x2254) = 0;
    m_turnRateAccumulator = 0;     // +0x2258
    *(int*)(p + 0x225C) = 0;
    *(int*)(p + 0x2260) = 0;
    *(int*)(p + 0x2264) = 0;
    *(int*)(p + 0x2268) = 1;
    m_flag226C = 0;                // +0x226C
    m_flag2270 = 0;                // +0x2270
    m_replayFlag = 0;              // +0x2274

    //  NTempest::CSyncData ("net sync events") - syncdata.h.  The real
    //  promotion site "Lead 1" (checksum-provider-registry.md) went
    //  looking for; see this file's own header for the naming note.
    m_syncData.m_vtable   = (void*)0x6F9581A4;
    m_syncData.m_checksum = 0;
    m_syncData.m_count    = 0;
    m_syncData.m_name     = "";   //  ValueName, 0x6F87529C - overwritten below
    m_sessionTag = kNetSessionTagNone;
    *(int*)(p + 0x228C) = 0;

    //  TInstanceRecycler<CNetData::CNetEvent> (+0x2290) - a real
    //  CDataRecycler(count=0x10, limit=0x40) base construction (already
    //  reconstructed, Net/cdatarecyclerctor.cpp), then the derived class's
    //  own vtable stamp - the standard two-step "construct base, then
    //  overwrite with the derived vtable" pattern, not the fully-inlined-
    //  away shape CNetCommandSave above uses, because this base's own
    //  constructor does real clamping work rather than nothing.
    new (p + 0x2290) CDataRecycler(0x10, 0x40);
    *(void**)(p + 0x2290) = (void*)kInstanceRecyclerVftable;

    *(int*)(p + 0x22A8) = 1;

    //  +0x22AC/+0x22B0 - see this file's own header comment: the real
    //  int-to-float normalize this reads two unidentified globals for is
    //  approximated as 0.0f rather than guessed at.
    *(float*)(p + 0x22AC) = 0.0f;
    *(float*)(p + 0x22B0) = 1.0f;
    *(int*)(p + 0x22B4) = 1;
    *(int*)(p + 0x22B8) = 1;
    *(int*)(p + 0x22BC) = 0;
    *(int*)(p + 0x22C0) = 0;

    memset(p + 0x1744, 0, 0xFF * 4);

    int turnStoreField10 = *(int*)(p + 0x1C88);
    *(int*)(p + 4) = 2;
    m_activeIndex = 0;             // +0x610
    *(int*)(p + 0x2244) = turnStoreField10;
    *(unsigned char*)(p + 0xAD8) = 0;
    *(int*)(p + 0x1C64) = 0;       //  the live per-tick sync value's own
                                    //  storage slot - checksum-provider-
                                    //  registry.md's own "LIVE CONFIRMATION"
                                    //  section.
    m_syncData.m_name = "net sync events";

    RegisterNetDataSingleton();
}

SNetSessionInfo::~SNetSessionInfo()
{
    char* p = (char*)this;

    *(void**)(p + 0) = (void*)kCNetDataVftable;

    //  0x6F54B850 - drain the +0xF38 event-queue header before it is torn
    //  down below.  See netdataeventqueueheader.cpp/this file's own
    //  DrainNetDataEventQueue declaration for why it stays a thunk.
    DrainNetDataEventQueue(this);

    //  TInstanceRecycler<CNetEvent> (+0x2290) - release through its own
    //  vtable slot 1 (raw dispatch: the concrete override this session did
    //  not identify), then the shared no-op (`nullsub_68`), then restamp
    //  and tear down the CDataRecycler base for real (already
    //  reconstructed).
    typedef void (__thiscall *RecyclerVSlot1Fn)(void*);
    void** recyclerVtable = *(void***)(p + 0x2290);
    ((RecyclerVSlot1Fn)recyclerVtable[1])(p + 0x2290);
    NullSub68();
    *(void**)(p + 0x2290) = (void*)kInstanceRecyclerVftable;
    ((CDataRecycler*)(p + 0x2290))->~CDataRecycler();

    //  NTempest::CSyncData (+0x2278) - the destructor never calls a
    //  separate ~CSyncData(): it is trivial and this toolchain inlines the
    //  base restamp directly, matching the shipped stream's own single
    //  `mov [reg], offset ??_7CEntity@NTempest@@6B@` exactly (see
    //  syncdata.h's own file header for the identification).
    *(void**)(p + 0x2278) = (void*)kNTempestCEntityVftable;

    //  Net::CTurnStore (+0x1C78) - the same base destructor already
    //  reused by the constructor.
    CDataStoreCache1460_DestroyInPlace(p + 0x1C78);

    //  TRefCnt base restamp for CNetCommandSave (+0x1B48) - ordinary
    //  mid-destructor base restamping, no separate ~TRefCnt() call needed
    //  (refcnt.h's own TRefCnt::~TRefCnt() is `{}`).
    *(void**)(p + 0x1B48) = (void*)kTRefCntVftable;
    //  The +0xF38 event-queue header's own destructor - see
    //  netdataeventqueueheader.cpp for why it stays a naked thunk.
    DestroyNetDataEventQueueHeader(p + 0xF38);

    SaveGameBaseDestruct(p + 0xBE8);

    if (*(void**)(p + 0xAB0))
        SMemFree(*(void**)(p + 0xAB0), ".?AUPLAYERDATA@Net@@", -2, 0);

    SaveGameBaseDestruct(p + 0x618);

    //  m_records[2] (+0x08) tears down automatically here - the compiler's
    //  own `eh vector destructor iterator`, matching the shipped stream.
}
