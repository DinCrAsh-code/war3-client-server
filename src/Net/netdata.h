//============================================================================
//  CNetPlayerRecord - the per-player sub-structure CNetData keeps at
//  `CNetData + 8 + player*0x304` (PauseGame_LAN_path_analysis.md's own
//  offset table).  Only the fields this repo's call trees actually reach are
//  named; everything else is `char m_reservedNN[...]` padding sized against
//  the 0x304 stride.  Extend this file (do not add a second declaration
//  elsewhere) as later modules reach more of it - see CLAUDE.md, "Struct
//  offsets are exact, and only touched members get names."
//
//  **The shipped binary's own name for this class is `CNetData::CNetSession`.**
//  0x6F53E0B0 - `SetGameState` below, whose `this` is exactly this +8 view -
//  refuses a transition by formatting the string at 0x6F9585D0:
//
//      "CNetData::CNetSession::SetGameState (from %s to %s)"
//
//  `CNetPlayerRecord` is this repo's own analysis name, committed first by
//  the CNetData wire-command target, and it stays for now because renaming
//  it moves a hookable key that belongs to that target.  A session that
//  wants to do it properly renames the class in one pass across
//  netcommand.h, netcommand_fire.cpp, netcommand_observers.cpp and the four
//  builder files, and only `CNetPlayerRecord::FireToObserverLists` changes
//  key: every other name in those files is a free function whose readable
//  name does not carry the type.
//
//  The game states are the binary's too, read out of 0x6F9582E0..0x6F958358
//  by 0x6F537930 (GameStateName): NONE, JOINED, FAKED, PLAYING, PAUSING,
//  PAUSED, with "GAME_STATE_UNKNOWN?" for 3 - which is in range, is
//  reachable (PLAYING is accepted from it) and has no name of its own.
//============================================================================
#ifndef NETDATA_H
#define NETDATA_H

#include "observer.h"
#include "netclient.h"        // CNetSendQueue
#include "gamecontext.h"        // GetThreadLocalSlot
#include "cdatastore.h"       // CDataStore - two embedded instances below
#include "syncdata.h"         // NTempest::CSyncData - SNetSessionInfo's own +0x2278

class CNetCommandBase;

enum
{
    kGameStateNone    = 0,
    kGameStateJoined  = 1,
    kGameStateFaked   = 2,
    kGameStatePlaying = 4,
    kGameStatePausing = 5,
    kGameStatePaused  = 6
};

//  0x6F537930 - the printable name of a game state (netgamestatename.cpp).
const char* __fastcall GameStateName(int state);

//  0x6F537840 - format into a 256-byte buffer and post it as a system
//  message of type 5 (netsysmessagef.cpp).
void SysMessagePrintf(const char* format, ...);

class CNetPlayerRecord
{
public:
    //  0x6F54FAB0 / 0x6F54FB90 - real reconstructions, netplayerrecordctor.cpp
    //  (cnd-ctor-agent-2 session).  The constructor placement-constructs a
    //  real `CNetObserver` (netobserver.h) directly over this object's own
    //  +0x00..+0x134 span - see that header for why the fields below keep
    //  their existing names/types rather than being retyped.
    CNetPlayerRecord();
    ~CNetPlayerRecord();

    //  0x6F5378E0 - fires `cmd` (treated generically as a SAgentMessage*)
    //  through both embedded observer lists' vtable slot 4
    //  (CObserver::Method_0x10); nonzero from either counts as delivered.
    //  See netcommand_fire.cpp.
    int FireToObserverLists(CNetCommandBase* cmd);

    //  0x6F5378C0 - hand a message straight to the UI observer list at
    //  +0x10 through its own slot 2, as a tail jump: the shipped body is
    //  four instructions and reuses the caller's three stack arguments.
    //  See netplayerrecord_uidispatch.cpp.
    void DispatchToUiObservers(unsigned int msg, unsigned int arg1,
                               void* arg2);

    //  0x6F53E0B0 - netdatasetgamestate.cpp.  The one member of this class
    //  whose `this` the shipped code actually forms with `lea ecx,[rec+8]`,
    //  and the function whose diagnostic names the class.
    int SetGameState(int state);

    //  0x6F54C380 - put the session back to its just-created state:
    //  netsessionclear.cpp.  Everything below +0x134 that it names is
    //  named because it names it.
    void Reset();

    //  The slot table at +0x288, which is CNetData's own m_slots eight
    //  bytes further along.  Reached by address (`lea ecx,[esi+288h]`), so
    //  it is an accessor rather than a member - CNetData::SLOTTABLE is
    //  declared below this class, and moving it out to make it a member
    //  would rename CNetData::SLOTTABLE::Find.
    void* SlotTable() { return m_slotTable; }

    char          m_reserved00[4];      // +0x00 - not reached by this module
    CObserver     m_pauseObserverList;  // +0x04
    CObserver     m_uiObserverList;     // +0x10
    char          m_reserved1C[0x134 - 0x1C];
    //  Two blocks Reset() clears with memset, sized exactly as the shipped
    //  calls size them.  Nothing here reads either.
    char          m_block134[0xB8];     // +0x134
    char          m_block1EC[0x82];     // +0x1EC
    char          m_reserved26E[0x270 - 0x26E];
    int           m_gameState;          // +0x270
    //  +0x274 - where Reset() parks the state it is about to drop.
    int           m_priorGameState;     // +0x274
    //  +0x278 - set to 1 by the constructor (netplayerrecordctor.cpp);
    //  not otherwise read/written in this closure, so its purpose past
    //  "constructor-set flag" is not established.
    int           m_field278;           // +0x278
    int           m_zero27C;            // +0x27C
    int           m_zero280;            // +0x280
    int           m_zero284;            // +0x284
    char          m_slotTable[0x4C];    // +0x288 - CNetData::SLOTTABLE
    unsigned char m_localSlot;          // +0x2D4 - CNetData's own +0x2DC
    unsigned char m_pausingSlot;        // +0x2D5
    char          m_reserved2D6[2];
    int           m_resumeBlocked;      // +0x2D8
    int           m_zero2DC;            // +0x2DC
    //  +0x2E0 - the suspended-player array.  Its Storm allocation tag is
    //  ".?AUSUSPENDPLAYER@Net@@", i.e. `struct Net::SUSPENDPLAYER`.
    unsigned int  m_suspendAlloc;       // +0x2E0
    unsigned int  m_suspendCount;       // +0x2E4
    void*         m_pSuspended;         // +0x2E8
    unsigned int  m_suspendChunk;       // +0x2EC
    int           m_zero2F0;            // +0x2F0
    //  +0x2F4 - the game-result array, tag ".?AUGAMERESULT@Net@@".  Each
    //  element is twenty bytes (netgameresultsubmit.cpp validates them by
    //  walking backwards in 0x14-byte steps).
    unsigned int  m_resultAlloc;        // +0x2F4
    unsigned int  m_resultCount;        // +0x2F8
    void*         m_pResults;           // +0x2FC
    //  +0x300 - zeroed by the constructor (netplayerrecordctor.cpp);
    //  not otherwise read/written in this closure.
    int           m_field300;           // +0x300
};

//----------------------------------------------------------------------------
//  CNetData - the 0x304-byte array element CNetPlayerRecord sits eight bytes
//  into.  The six free `__fastcall` helpers of the module index *this*
//  record, not the +8 view: every one of them forms `record + index*0x304`
//  and then reads +0x278, +0x290, +0x2DC, +0x2DD or +0x2E0 straight off it,
//  and only SetGameState above gets the `lea ecx,[record+8]`.  Modelling the
//  two as one class costs every one of those displacements its match, which
//  is what the eight-byte difference in the diff was before this split.
//
//  Its own gaps are exact and sized against the 0x304 stride; only the
//  fields this closure reads are named.
//----------------------------------------------------------------------------
//  A user report (2026-09-13) flagged this class as wrongly missing
//  `: public CSynchronousData`, per the real RTTI
//  (`agent_worktrees/classes/0x6F9590D0.json`, `hierarchy_raw: "CNetData:
//  CSynchronousData;"`) and a much larger real constructor allocation
//  (reported ~0x23C8 bytes) than this struct's own 0x304.  Both are real,
//  but **do not apply to this struct** - checksum-provider-registry.md's
//  own "netdatachecksum"/"provider-slots-2" batches already traced this in
//  full: the RTTI-named, `CSynchronousData`-deriving, ~0x23C8-byte
//  `CNetData` is the *array-base* singleton (`CNetData::CNetData`,
//  `sub_6F54FC50`, stamps `??_7CNetData@@6B@` at its own offset 0 - real,
//  polymorphic) that `m_pCache[2]` (net checksum-provider slot 2) points
//  at directly, the same base pointer this file's own `NetDataRecords()`
//  calls `m_pRecords`.  Its own `+0x2278` embeds a further RTTI-confirmed
//  sub-object (`NTempest::CSyncData`, per the "LIVE FINDING" section of
//  that doc, and per the cnd-ctor-agent session below, directly confirmed
//  by the real constructor's own `+0x2278` vtable stamp) and its layout
//  runs on through everything `SNetSessionInfo` below models from the same
//  base, plus more not yet reconstructed here - 0x23C8 is very plausibly
//  that whole object's real size, not this struct's.
//
//  **Correction (cnd-ctor-agent session, 2026-09-13):** the constructor is
//  `sub_6F550460`, not `sub_6F54FC50` - the two were swapped in this
//  comment and in checksum-provider-registry.md's own "Lead 1" section
//  until this session read both bodies start to finish.  `sub_6F54FC50` is
//  `~CNetData()`; see checksum-provider-registry.md's "cnd-ctor-agent
//  session" for the full read of both, including why "Lead 1"'s own
//  "promotion site never found" conclusion was actually just the
//  destructor's ordinary mid-unwind restamp of `+0x2278` down to
//  `NTempest::CEntity`, not a second, missing construction site.
//
//  *This* struct is the unrelated 0x304-byte-stride *element type* the six
//  static helpers below index the array with (`record + index*0x304`,
//  always index 0 in every call site this repo has), non-polymorphic by
//  construction - nothing here stamps a vtable, and dozens of
//  already-`EXACT`/`IDENTICAL`-scored functions read its fields at the
//  exact byte offsets below on that assumption. Retroactively giving *this*
//  struct a `CSynchronousData` base would insert a 4-byte vtable pointer at
//  offset 0 and silently shift every one of those offsets by 4 bytes -
//  exactly the "too large a blast radius for a leaf nobody has
//  reconstructed the body of yet" call the netdatachecksum session already
//  made and documented. Left as-is; the real fix (giving the array-base
//  singleton its own proper `class CNetData : public CSynchronousData`
//  distinct from this per-record element struct, once its own body is
//  worth reconstructing) is future work, not a same-day one-line patch.
class CNetData
{
public:
    //------------------------------------------------------------------
    //  One participant of the session, found through the slot table below.
    //  Only the counter the pause path bumps is named.
    //------------------------------------------------------------------
    //  The hash-table node type the slot table's own buckets thread.  Its
    //  name is the shipped one: the player-pointer array below is freed
    //  under the Storm allocation tag ".PAUCPlayer@CNetData@@", i.e.
    //  `struct CNetData::CPlayer *`.  Nothing here reads a field of one, so
    //  it stays incomplete.
    struct CPlayer;

    struct SLOT
    {
        char m_reserved00[0x38];
        int  m_pauseCount;              // +0x38
    };

    //------------------------------------------------------------------
    //  Two parallel pointer arrays, selected by the slot byte's top bit:
    //  clear picks the players, set picks the referees/observers and the
    //  remaining seven bits index them.
    //------------------------------------------------------------------
    class SLOTTABLE
    {
    public:
        //  0x6F54F430 / 0x6F54FA00 - real reconstructions,
        //  netdataslottable_ctor.cpp (cnd-ctor-agent-2 session).
        SLOTTABLE();
        ~SLOTTABLE();

        //  0x6F53E050 - netdataslotfind.cpp.
        SLOT* Find(unsigned char slot);

        //  0x6F54B1A0 - drop every node, either by unlinking it or by
        //  handing it to the table's own vtable slot 0.  `retn 4`.
        //  netsessionslottableclear.cpp.
        void ReleaseAll(int unlinkOnly);

        //  The first 0x28 bytes are Storm's TSHashTable header exactly -
        //  vtable, the table-wide list, the chain-length counter, the
        //  bucket array and the mask - which is what ReleaseAll's own
        //  `[edi+10h] = 0`, `UnlinkAll(edi+4)` and `[edi+18h]`/`[edi+1Ch]`
        //  bucket walk say.  The two slot arrays follow it; each is an
        //  ordinary {alloc, count, data, chunk} four-word array, and the
        //  count/data pairs Find() already used are its middle two words.
        void*         m_vtable;         // +0x00
        int           m_listLinkOffset; // +0x04
        void*         m_listNext;       // +0x08
        int           m_listPrevlink;   // +0x0C
        unsigned int  m_probes;         // +0x10
        unsigned int  m_bucketAlloc;    // +0x14
        unsigned int  m_bucketCount;    // +0x18
        void*         m_buckets;        // +0x1C
        unsigned int  m_bucketChunk;    // +0x20
        unsigned int  m_mask;           // +0x24
        unsigned int  m_playerAlloc;    // +0x28
        unsigned int  m_playerCount;    // +0x2C
        SLOT**        m_players;        // +0x30
        unsigned int  m_playerChunk;    // +0x34
        unsigned int  m_refereeAlloc;   // +0x38
        unsigned int  m_refereeCount;   // +0x3C
        SLOT**        m_referees;       // +0x40
        unsigned int  m_refereeChunk;   // +0x44
        unsigned int  m_field48;        // +0x48
    };

    //  The +8 view, i.e. the receiver SetGameState is called on.
    CNetPlayerRecord* Session()
    {
        return (CNetPlayerRecord*)((char*)this + 8);
    }

    //  The six helpers.  `__fastcall`, no receiver, and the record index
    //  they take is the array subscript - always 0 at every call site in
    //  this closure - so they are statics.
    //  0x6F53ED30 / 0x6F53EDD0 - netdataslots.cpp.
    static unsigned char __fastcall GetPausingSlot(int index);
    static unsigned char __fastcall GetLocalSlot(int index);
    //  0x6F53EE90 - netdataslotpausecount.cpp.
    static int __fastcall GetSlotPauseCount(unsigned char slot, int index);
    //  0x6F53EED0 - netdataslotparticipating.cpp.
    static int __fastcall IsSlotParticipating(unsigned char slot, int index);
    //  0x6F53EA60 / 0x6F53EAF0 - netdatapause.cpp.
    static int __fastcall RequestPause(unsigned char slot, int index,
                                       int countIt);
    static int __fastcall RequestResume(int index);

    char          m_reserved000[0x278];
    int           m_gameState;          // +0x278 = the session's own +0x270
    char          m_reserved27C[0x290 - 0x27C];
    SLOTTABLE     m_slots;              // +0x290 - 0x4C bytes, ending at
                                        //   +0x2DC exactly
    unsigned char m_localSlot;          // +0x2DC
    unsigned char m_pausingSlot;        // +0x2DD
    char          m_reserved2DE[0x02];
    int           m_resumeBlocked;      // +0x2E0
    //  +0x2E4 - CNetPlayerRecord's own m_zero2DC (that class's +0x2DC, this
    //  one's own view being CNetPlayerRecord's -8) read/written by
    //  CNetData::NotifyResyncComplete (netcommand_dispatch.cpp) as a
    //  genuine "save still pending" flag, not the always-0 field the other
    //  class's own name still describes accurately for every builder that
    //  touches it.
    int           m_saveStillPending;   // +0x2E4
    char          m_reserved2E8[0x1C];
};

//----------------------------------------------------------------------------
//  The record array, reached the way every other thread-local game-data read
//  in this binary is (gamecontext.h, gamedatatag.cpp): slot 0x0D of this
//  thread's pointer array, then +0x10, then +0x08.  An inline because the
//  six helpers each carry their own copy of the sequence in the shipped
//  stream.
//----------------------------------------------------------------------------
struct SNetDataCacheSlot
{
    char       m_reserved00[0x08];
    CNetData*  m_pRecords;              // +0x08
};

struct SNetDataSlotRoot
{
    char                m_reserved00[0x10];
    SNetDataCacheSlot*  m_pCache;       // +0x10
};

//----------------------------------------------------------------------------
//  The record array's base is also where the session's own fields live: the
//  four-character tag at +0x2288 is 0x2288 bytes past it, well past the
//  0x304-byte records this closure indexes, and is the only field of it
//  anything here reads.  Modelled as its own view rather than as a member
//  of CNetData, whose size is fixed at 0x304 by the stride.
//----------------------------------------------------------------------------
struct SNetSessionInfo
{
    //  0x6F54B080 - ship whatever game results record `index` has
    //  accumulated to the network client and drop the array.  `retn 4`,
    //  `this` in ecx.  Net/netgameresultsubmit.cpp.
    void SubmitGameResults(int index);

    char             m_reserved0000[8];
    //  +0x08 - the two-element CNetPlayerRecord array CNetData::CNetData
    //  (0x6F550460, netdatactor.cpp, cnd-ctor-agent-2 session) builds
    //  through the compiler's own `eh vector constructor iterator` - a
    //  real C++ array of a class with a real constructor/destructor
    //  reproduces that call exactly, rather than this file calling the
    //  iterator by hand.  0x304 * 2 = 0x608 bytes, ending +0x610 exactly -
    //  matches CNetPlayerRecord's own declared size (netdata.h, above)
    //  and the array-of-`CNetData`-stride model that class's own header
    //  comment already documents: `NetDataRecords()`'s `CNetData*` is this
    //  same base address, reinterpreted.
    CNetPlayerRecord m_records[2];      // +0x08, ending +0x610
    //  +0x610 - which record index is the live one.  0x6F54C7E0 does
    //  nothing at all unless the index it was handed matches this.
    int          m_activeIndex;         // +0x610
    char         m_reserved0614[0x618 - 0x614];
    //  +0x618 - an embedded CDataStore (netevent_dispatch.cpp,
    //  CNetEventTypeDispatcher::ConstructAndBroadcast /
    //  CNetEventTypeDispatcher::ReplayAckDispatch, 0x6F551D80/0x6F54EB00) -
    //  the replay/turn stream both functions pull queued records off of.
    //  `[self+0x618]`'s own vtable slot 7 (`+0x1C`) is called to close the
    //  stream on the "no more data" transition (SetReplayState(3) below),
    //  and slot 8 (`+0x20`) elsewhere - neither slot is in CDataStore's own
    //  declared virtual set (only slots 0/1/Grow are named there), so both
    //  calls go through the raw vtable-index shape CLAUDE.md's own example
    //  uses rather than adding an unconfirmed virtual to that class.
    //  `m_field10`/`m_readPos` (this object's own +0x10/+0x14, i.e.
    //  absolute +0x628/+0x62C) are exactly the fields
    //  ReplayAckDispatch's own jump-table decode compares to detect
    //  "stream exhausted", the same shape every other CDataStore reader
    //  already uses.
    CDataStore   m_replayStream;         // +0x618, 0x18 bytes
    char         m_reserved0630[0xABC - 0x630];
    //  +0xABC - gates the two "trusted" CNetEvent cases (48/49,
    //  CNetEventTrustedDesync/CNetEventTrustedResult) in
    //  CNetEventTypeDispatcher::ConstructAndBroadcast.
    int          m_trustedEventsEnabled; // +0xABC
    char         m_reserved0AC0[0xAC4 - 0xAC0];
    //  +0xAC4 - read (not written) by ConstructAndBroadcast's type-0x10/
    //  0x15 route-candidate-probe branch when the probe's own bit-0x100
    //  flag is set, and copied into m_cheatsEnabled below - an overload of
    //  that field this branch's own call site is the only reader/writer
    //  of; see that function's own header comment.
    int          m_routeCandidateAltFlag; // +0xAC4
    //  +0xAC8/+0xACC/+0xAD0/+0xAD4 - ConstructAndBroadcast's own
    //  route-candidate/reconnection bookkeeping for the type-0x10/0x15/0x1B
    //  inline special cases (see that function's own header comment).
    //  m_routeRetry mirrors whatever ReplayAckDispatch leaves in its own
    //  `[self+0xAC8]` scratch slot; m_routeReplayArg is a snapshot of
    //  m_replayStream.m_field10 taken right before a ReplayAckDispatch call.
    int          m_routeRetry;           // +0xAC8
    unsigned int m_routeReplayArg;       // +0xACC
    int          m_routeFlagAD0;         // +0xAD0
    int          m_routeFlagAD4;         // +0xAD4
    char         m_reserved0AD8[0xBE0 - 0xAD8];
    //  +0xBE0 - a route-candidate readiness flag ConstructAndBroadcast's
    //  type-0x10/0x15/0x1B special cases and its shared tail all gate on.
    int          m_flagBE0;              // +0xBE0
    //  +0xBE4 - a per-session flag DispatchQueuedTurnRecord
    //  (netcommand_dispatch.cpp, 0x6F5516E0) compares against the literal
    //  1 (`cmp [esi+0BE4h], 1; setz dl`) as one of the three ORed
    //  conditions gating whether it fires a tick-notification event
    //  (sub_6F545840).  ConstructAndBroadcast's type-0x1B case also reads
    //  it (`== 1` selects the SetReplayState(2)/SetRecordState(1) path).
    int          m_flagBE4;             // +0xBE4
    //  +0xBE8 - a second embedded CDataStore, the "UI/observer" stream
    //  ConstructAndBroadcast tears down through the same vtable slot 7 on
    //  every dispatch (unconditionally, not just the type-0x1B path).
    CDataStore   m_observerStream;        // +0xBE8, 0x18 bytes
    char         m_reserved0C00[0x1B44 - 0xC00];
    //  +0x1B44 - cleared at CNetData::DispatchActionByte's own entry and
    //  set to 1 on jump-table index 6's own special-cased body (the one
    //  case that does not call a CNetCommandXxx builder at all - see
    //  netcommand_dispatch.cpp).  Read nowhere else in this closure, so
    //  its purpose ("a per-session command was just latched") is inferred
    //  from the write site, not proven by a reader.
    int          m_dispatchLatched;      // +0x1B44
    //  +0x1B48 - a slot the same special case hands to sub_6F5547C0
    //  (thunked, netcommand_dispatch.cpp) by address; not independently
    //  read anywhere in this closure, so its contents are opaque.
    char         m_reserved1B48[0x1B58 - 0x1B48];
    //  +0x1B58 / +0x1B5D - the player-index/sender pair jump-table index
    //  6's special case latches alongside m_dispatchLatched above, through
    //  the same three stores (`mov [esi+1B58h],edi` / `mov [esi+1B5Dh],cl`).
    unsigned int  m_dispatchLatchedPlayerIndex;  // +0x1B58
    char          m_reserved1B5C;
    unsigned char m_dispatchLatchedSender;       // +0x1B5D
    char         m_reserved1B5E[0x1C68 - 0x1B5E];
    //  +0x1C68 - the "turn" value every one of DispatchActionByte's 133
    //  case bodies pushes as the `turn` argument to its builder, and the
    //  same field DispatchQueuedTurnRecord (0x6F5516E0) increments by one
    //  on the "no explicit player index" path before calling in.  Named
    //  from that use, not independently confirmed as a wire turn number.
    unsigned int m_currentTurn;          // +0x1C68
    char         m_reserved1C6C[0x1C74 - 0x1C6C];
    //  +0x1C74 - a session-wide "cheats permitted" flag: DispatchActionByte
    //  only lets its cheat-permission gate's fallback branch force
    //  suppression when this is nonzero *and* the raw wire action byte
    //  falls in [0x20,0x32] - see docs/targets/CNetData_DispatchActionByte.md.
    int          m_cheatsEnabled;        // +0x1C74
    //  +0x1C78 - the outbound send queue 0x6F54C7E0 drains and then closes
    //  through its own vtable slot 7 before the results go out.
    CNetSendQueue m_sendQueue;          // +0x1C78
    char         m_reserved1C7C[0x2258 - 0x1C7C];
    //  +0x2258 - a per-session running count DispatchQueuedTurnRecord
    //  recomputes as `(x*8 - x + delta) / 8` (a `x * 7/8 + delta/8`-shaped
    //  moving update) whenever a latched player index was supplied.
    unsigned int m_turnRateAccumulator;  // +0x2258
    char         m_reserved225C[0x226C - 0x225C];
    //  +0x226C / +0x2270 - two more flags DispatchQueuedTurnRecord ORs
    //  into the same tick-notification gate as m_flagBE4 above.
    int          m_flag226C;             // +0x226C (checked !=0 first)
    int          m_flag2270;             // +0x2270 (checked !=0 second)
    //  +0x2274 - the `replayFlag`/verbose argument every one of
    //  DispatchActionByte's case bodies passes to its builder (the
    //  `verbose` parameter CNetData_FireCommand logs under).
    int          m_replayFlag;           // +0x2274
    //  +0x2278 - the "net sync events" CSyncData (syncdata.h,
    //  NTempest::CSyncData) - see the constructor's own file
    //  (netdatactor.cpp, cnd-ctor-agent-2 session) for the promotion
    //  site this had gone looking for; syncdata.h's own file header has
    //  the full identification.
    CSyncData    m_syncData;            // +0x2278, 0x10 bytes
    unsigned int m_sessionTag;          // +0x2288

    //  0x6F550460 / 0x6F54FC50 - real reconstructions, netdatactor.cpp
    //  (cnd-ctor-agent-2 session).  See that file for why this struct -
    //  rather than a class actually named `CNetData` - carries the real
    //  array-base singleton's own constructor/destructor: `class CNetData`
    //  above this file is already a different, non-polymorphic type (the
    //  0x304-byte-stride per-record element the shipped binary calls
    //  `CNetData::CNetSession`), and this file's own top-of-class note
    //  already explains why the two are kept apart rather than merged.
    SNetSessionInfo();
    ~SNetSessionInfo();
};

//----------------------------------------------------------------------------
//  The per-slot route/replay record CNetEventTypeDispatcher::
//  ConstructAndBroadcast (0x6F551D80) reads at `self + 0x278 + slot*0x304`
//  for its type-0x10/0x15/0x1B inline special cases - a *different* array
//  from CNetData's own player table (that one is reached through the
//  thread-local slot root above, not through `self` here).  Not declared
//  as a member array of SNetSessionInfo itself: `(0x610-0x278)/0x304` is
//  not a whole number of slots, so the region between +0x278 and +0x610
//  stays anonymous reserved space and this is reached the same way
//  CNetData's own six static per-index helpers already are - by raw
//  offset arithmetic, not a declared array.  Only the three members this
//  dispatcher's type-0x1B case actually reads get names.
//----------------------------------------------------------------------------
struct SNetSlotRouteRecord
{
    int  m_active;                       // +0x00 (absolute +0x278 + slot*0x304)
    char m_reserved04[0x288 - 0x278 - 4];
    //  +0x10 (absolute +0x288) - nonzero selects the "streaming" replay
    //  transition (SetReplayState(2)); zero selects "no more data"
    //  (SetReplayState(3) + stream teardown).
    int  m_kind;                         // +0x10
    char m_reserved14[0x2D8 - 0x288 - 4];
    //  +0x60 (absolute +0x2D8) - "streaming just activated": when nonzero
    //  on the SetReplayState(2) path, ConstructAndBroadcast also invokes
    //  CNetEventTypeDispatcher::ReplayAckDispatch (0x6F54EB00).
    int  m_streamingJustActivated;       // +0x60
};

inline SNetSlotRouteRecord* SlotRouteRecord(SNetSessionInfo* self, unsigned int slot)
{
    return (SNetSlotRouteRecord*)((char*)self + 0x278 + slot * 0x304);
}

//: 'NONE' and 'BNET', as the shipped code spells them - four-character tags
//: stored with the first character in the low byte.
const unsigned int kNetSessionTagNone = 0x4E4F4E45u;
const unsigned int kNetSessionTagBattleNet = 0x424E4554u;

//  0x6F53E640 - Net/netsessiontag.cpp.
unsigned int __fastcall GetNetSessionTag();

//  0x6F53F350 / 0x6F53F3C0 / 0x6F53F400 - Net/netuidispatch.cpp and
//  Net/netuidispatchclass.cpp.
void __fastcall DispatchSessionUiMessage(unsigned int msg, unsigned int arg1,
                                         void* arg2, int index);
void __fastcall DispatchSessionUiMessageA(unsigned char id, unsigned int arg1,
                                          void* arg2, int index);
void __fastcall DispatchSessionUiMessageB(unsigned char id, unsigned int arg1,
                                          void* arg2, int index);

inline CNetData* NetDataRecords()
{
    SNetDataSlotRoot* root = (SNetDataSlotRoot*)GetThreadLocalSlot(0x0D);
    return root->m_pCache->m_pRecords;
}

#endif
