//============================================================================
//  0x6F54CD40 - CBattleNetEventTypeDispatcher::ConstructAndBroadcast.
//  cnd-wave2-b's own target: CNetData_DispatchOneQueuedRecord
//  (0x6F5530D0, Net/pausecallchainthunks.cpp) calls this one for every
//  queued record whose type-tag byte is >= 0x40 - the Battle.net-event
//  mirror of CNetEventTypeDispatcher::ConstructAndBroadcast (0x6F551D80,
//  Net/netevent_dispatch.cpp), which handles everything below 0x40.
//
//  Shape, read straight off the dump (no dump file committed here - IDA
//  address only, per `worktree_store.py show`):
//
//    1. Wrap the raw record ([arg_0+8]=pointer,[arg_0+0Ch]=length) in a
//       stack CDataStore (`store` below), the same "wrap for reading"
//       shape every other CDataStore wrapper in this closure uses.
//    2. Remap the record's own type-tag byte ([arg_0+0x14]) into a 0-63
//       switch index: `((rawByte - 0x40) clamped to [0,0xBE]) - 1`, so raw
//       bytes 0x41-0x80 map to index 0-63 and everything else (including
//       exactly 0x40) forces an out-of-range index that falls to default.
//    3. The record's slot is self->m_activeIndex unless the record's own
//       +0x15 byte is 0 (bare record, slot 0) - the same convention
//       CNetEventTypeDispatcher::ConstructAndBroadcast's own slot read
//       uses.
//    4. 50 of the 64 index values each placement-construct one
//       CBattleNetEventXxx object at self+8+slot*0x304 (the same
//       CNetPlayerRecord-array slot ConstructAndBroadcast's own ~40
//       uniform CNetEvent cases write into - both dispatchers stamp
//       whichever concrete type last ran into the identical per-slot
//       memory) and pass it on, with `this`=that address in ecx, `slot` in
//       edx, and 7 stack args (&store, the type's own class-name string
//       literal, 0,0,0,0,0), `retn 0x1Ch` - confirmed identical for all 50
//       via `worktree_store.py bytes <addr> --offset -2`.  One index (43,
//       raw type 0x6B) is a genuinely different shape (a "download file"
//       special case - see below).  The other 14 fall to the shared tail
//       untouched (unhandled by design, not a gap: `0x54CDE1 ja
//       def_6F54CDE7` already means every one of them, plus every raw
//       byte the remap forces out of [0,63], reaches the same place).
//    5. The shared tail (every populated case, the special case, and every
//       unhandled index all converge here): placement-construct a base
//       ??_7TRefCnt@@6B@-vtable'd local, and if the record's own +0x15
//       byte cleared an "extra data" flag earlier (recorded in that
//       local's own +0xC field, compared against -1), hand it, the
//       original record's +8/+0xC pair and &store to a shared cleanup
//       entry point through a raw cross-module function pointer
//       (off_6F932C0C - the exact same pool-recycle pointer CDataStore's
//       own destructor, cdatastore.h, and the not-reconstructed write-side
//       CDataStore family both already use).
//
//  Every one of the 50 uniform-shaped constructors is left as a naked,
//  correctly-ABI'd thunk to its real, unhooked body - exactly the
//  convention CNetEventTypeDispatcher::ConstructAndBroadcast's own ~40
//  uniform CNetEvent constructors already established (netevent_dispatch.
//  cpp's own header comment on that block): reproducing 50 more distinct
//  constructor bodies (each almost certainly reads a couple more wire
//  fields the way that dispatcher's own CNetEventXxx subclasses do) is its
//  own multi-wave target, squarely out of this session's ~20-35-function
//  budget, not a gap in this function's own reconstruction. This function
//  itself - the switch skeleton, the slot/gating logic, and the special
//  case - is the real, unhooked-nowhere reconstruction; only its 50 leaf
//  callees are placeholders.
//============================================================================
#include "netdata.h"
#include "netcommand.h"      // CNetData_ParseError
#include "cdatastore.h"

#define BATTLENET_CTOR_THUNK(name, addr)                                     \
    __declspec(naked) void __fastcall name(                                  \
        void* thisObj, unsigned int slot, void* store, const char* eventName,\
        int zeroA, int zeroB, int zeroC, int zeroD, int zeroE)                \
    {                                                                         \
        __asm { mov eax, addr }                                              \
        __asm { jmp eax }                                                    \
    }

//  The 50 uniform CBattleNetEventXxx constructors - see the header comment
//  above.  `retn 0x1Ch` confirmed for every one of them individually.
BATTLENET_CTOR_THUNK(BattleNetEventConnect_Ctor_6F546F60,            0x6F546F60)
BATTLENET_CTOR_THUNK(BattleNetEventDisconnect_Ctor_6F53A3C0,         0x6F53A3C0)
BATTLENET_CTOR_THUNK(BattleNetEventLogon_Ctor_6F547090,              0x6F547090)
BATTLENET_CTOR_THUNK(BattleNetEventCreate_Ctor_6F53A4B0,             0x6F53A4B0)
BATTLENET_CTOR_THUNK(BattleNetEventChange_Ctor_6F53A5A0,             0x6F53A5A0)
BATTLENET_CTOR_THUNK(BattleNetEventUpgradeUpdate_Ctor_6F53A690,      0x6F53A690)
BATTLENET_CTOR_THUNK(BattleNetEventDownloadUpgradeFile_Ctor_6F5471B0,0x6F5471B0)
BATTLENET_CTOR_THUNK(BattleNetEventCheckAd_Ctor_6F5472D0,            0x6F5472D0)
BATTLENET_CTOR_THUNK(BattleNetEventQueryAdUrl_Ctor_6F547400,         0x6F547400)
BATTLENET_CTOR_THUNK(BattleNetEventNewsStart_Ctor_6F53A780,          0x6F53A780)
BATTLENET_CTOR_THUNK(BattleNetEventNewsStop_Ctor_6F53A870,           0x6F53A870)
BATTLENET_CTOR_THUNK(BattleNetEventNewsAdd_Ctor_6F547520,            0x6F547520)
BATTLENET_CTOR_THUNK(BattleNetEventFriendsStart_Ctor_6F53A960,       0x6F53A960)
BATTLENET_CTOR_THUNK(BattleNetEventFriendsStop_Ctor_6F53AA50,        0x6F53AA50)
BATTLENET_CTOR_THUNK(BattleNetEventFriendsAdd_Ctor_6F54A500,         0x6F54A500)
BATTLENET_CTOR_THUNK(BattleNetEventFriendsRemove_Ctor_6F53AB40,      0x6F53AB40)
BATTLENET_CTOR_THUNK(BattleNetEventFriendsPosition_Ctor_6F53AC30,    0x6F53AC30)
BATTLENET_CTOR_THUNK(BattleNetEventFriendsUpdate_Ctor_6F547640,      0x6F547640)
BATTLENET_CTOR_THUNK(BattleNetEventReadUserData_Ctor_6F54A630,       0x6F54A630)
BATTLENET_CTOR_THUNK(BattleNetEventProfile_Ctor_6F548060,            0x6F548060)
BATTLENET_CTOR_THUNK(BattleNetEventClanProfile_Ctor_6F548190,        0x6F548190)
BATTLENET_CTOR_THUNK(BattleNetEventCheckDataFile_Ctor_6F53AD20,      0x6F53AD20)
BATTLENET_CTOR_THUNK(BattleNetEventEnterChat_Ctor_6F547760,          0x6F547760)
BATTLENET_CTOR_THUNK(BattleNetEventChannelList_Ctor_6F54B3F0,        0x6F54B3F0)
BATTLENET_CTOR_THUNK(BattleNetEventChatEvent_Ctor_6F547890,          0x6F547890)
BATTLENET_CTOR_THUNK(BattleNetEventMessageBox_Ctor_6F5479C0,         0x6F5479C0)
BATTLENET_CTOR_THUNK(BattleNetEventTeamGetPotentials_Ctor_6F54A850,  0x6F54A850)
BATTLENET_CTOR_THUNK(BattleNetEventAnonProfile_Ctor_6F54C9A0,        0x6F54C9A0)
BATTLENET_CTOR_THUNK(BattleNetEventAnonProfileClan_Ctor_6F54A730,    0x6F54A730)
BATTLENET_CTOR_THUNK(BattleNetEventClanGetPotentials_Ctor_6F54A950,  0x6F54A950)
BATTLENET_CTOR_THUNK(BattleNetEventClanCreate_Ctor_6F547AF0,         0x6F547AF0)
BATTLENET_CTOR_THUNK(BattleNetEventClanFounder_Ctor_6F54AA50,        0x6F54AA50)
BATTLENET_CTOR_THUNK(BattleNetEventClanDisband_Ctor_6F53AE10,        0x6F53AE10)
BATTLENET_CTOR_THUNK(BattleNetEventClanDesignate_Ctor_6F53AF00,      0x6F53AF00)
BATTLENET_CTOR_THUNK(BattleNetEventClanAdded_Ctor_6F53AFF0,          0x6F53AFF0)
BATTLENET_CTOR_THUNK(BattleNetEventClanRemoved_Ctor_6F53B0E0,        0x6F53B0E0)
BATTLENET_CTOR_THUNK(BattleNetEventClanMemberAdd_Ctor_6F53B1D0,      0x6F53B1D0)
BATTLENET_CTOR_THUNK(BattleNetEventClanMemberRemove_Ctor_6F53B2C0,   0x6F53B2C0)
BATTLENET_CTOR_THUNK(BattleNetEventClanMemberInvite_Ctor_6F547C00,   0x6F547C00)
BATTLENET_CTOR_THUNK(BattleNetEventClanMemberRank_Ctor_6F53B3B0,     0x6F53B3B0)
BATTLENET_CTOR_THUNK(BattleNetEventClanInfoGet_Ctor_6F547D20,        0x6F547D20)
BATTLENET_CTOR_THUNK(BattleNetEventClanListStart_Ctor_6F53B4A0,      0x6F53B4A0)
BATTLENET_CTOR_THUNK(BattleNetEventClanListStop_Ctor_6F53B590,       0x6F53B590)
BATTLENET_CTOR_THUNK(BattleNetEventClanListRemove_Ctor_6F547E40,     0x6F547E40)
BATTLENET_CTOR_THUNK(BattleNetEventClanListUpdate_Ctor_6F54ABA0,     0x6F54ABA0)
BATTLENET_CTOR_THUNK(BattleNetEventClanChangeRank_Ctor_6F547F50,     0x6F547F50)
BATTLENET_CTOR_THUNK(BattleNetEventTournamentInfo_Ctor_6F54B4F0,     0x6F54B4F0)
BATTLENET_CTOR_THUNK(BattleNetEventScheduledGame_Ctor_6F54B5F0,      0x6F54B5F0)
BATTLENET_CTOR_THUNK(BattleNetEventMatchmakingInfo_Ctor_6F53B680,    0x6F53B680)
BATTLENET_CTOR_THUNK(BattleNetEventIconList_Ctor_6F54ACD0,           0x6F54ACD0)

#undef BATTLENET_CTOR_THUNK

//  Case 43 (raw type 0x6B) only - two more unmodeled leaves, thunked with
//  only the ABI their own call sites prove (no confirmed retn byte count
//  past what the disassembly shows; declared __fastcall/no-stack since
//  neither call site pushes anything beyond the two register args).
//  Genuinely out of scope: their own bodies are not part of this session's
//  50-constructor closure and nothing here derives what `localWireIdBuf`/
//  `remoteFileId` really are past "some small buffer sub_6F544AF0/
//  sub_6F6878D0 fill in".
extern "C" __declspec(naked) void __fastcall
ConstructDownloadFileWireIdBuf_6F544AF0(void* buf, unsigned int wireId)
{
    __asm { mov eax, 0x6F544AF0 }
    __asm { jmp eax }
}

extern "C" __declspec(naked) void __fastcall
ReadDownloadFileRecord_6F6878D0(void* store, void* out)
{
    __asm { mov eax, 0x6F6878D0 }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F54CD40.  `retn 4`.
//----------------------------------------------------------------------------
int __fastcall CBattleNetEventTypeDispatcher_ConstructAndBroadcast(
    SNetSessionInfo* self, void* /*unused_edx*/, void* record)
{
    CDataStore store;
    store.m_field4   = *(void**)((char*)record + 8);
    store.m_field8   = 0;
    store.m_field0xC = -1;
    store.m_field10  = *(int*)((char*)record + 0xC);
    store.m_readPos  = 0;

    unsigned char rawType = *((unsigned char*)record + 0x14);

    unsigned int clamped = ((unsigned int)(unsigned char)(rawType - 0x40) > 0xBE)
                               ? 0x41u
                               : (unsigned int)(unsigned char)(rawType - 0x40);
    int index = (int)clamped - 1;

    unsigned int slot = (*((unsigned char*)record + 0x15) != 0) ? self->m_activeIndex : 0;
    void* eventSlot = (char*)self + 8 + slot * 0x304;

    if ((unsigned int)index <= 0x3F)
    {
        switch (index)
        {
        case 1:  BattleNetEventConnect_Ctor_6F546F60(eventSlot, slot, &store, "CBattleNetEventConnect", 0,0,0,0,0); break;
        case 2:  BattleNetEventDisconnect_Ctor_6F53A3C0(eventSlot, slot, &store, "CBattleNetEventDisconnect", 0,0,0,0,0); break;
        case 4:  BattleNetEventLogon_Ctor_6F547090(eventSlot, slot, &store, "CBattleNetEventLogon", 0,0,0,0,0); break;
        case 5:  BattleNetEventCreate_Ctor_6F53A4B0(eventSlot, slot, &store, "CBattleNetEventCreate", 0,0,0,0,0); break;
        case 6:  BattleNetEventChange_Ctor_6F53A5A0(eventSlot, slot, &store, "CBattleNetEventChange", 0,0,0,0,0); break;
        case 7:  BattleNetEventUpgradeUpdate_Ctor_6F53A690(eventSlot, slot, &store, "CBattleNetEventUpgradeUpdate", 0,0,0,0,0); break;
        case 8:  BattleNetEventDownloadUpgradeFile_Ctor_6F5471B0(eventSlot, slot, &store, "CBattleNetEventDownloadUpgradeFile", 0,0,0,0,0); break;
        case 9:  BattleNetEventCheckAd_Ctor_6F5472D0(eventSlot, slot, &store, "CBattleNetEventCheckAd", 0,0,0,0,0); break;
        case 10: BattleNetEventQueryAdUrl_Ctor_6F547400(eventSlot, slot, &store, "CBattleNetEventQueryAdUrl", 0,0,0,0,0); break;
        case 11: BattleNetEventNewsStart_Ctor_6F53A780(eventSlot, slot, &store, "CBattleNetEventNewsStart", 0,0,0,0,0); break;
        case 12: BattleNetEventNewsStop_Ctor_6F53A870(eventSlot, slot, &store, "CBattleNetEventNewsStop", 0,0,0,0,0); break;
        case 13: BattleNetEventNewsAdd_Ctor_6F547520(eventSlot, slot, &store, "CBattleNetEventNewsAdd", 0,0,0,0,0); break;
        case 14: BattleNetEventFriendsStart_Ctor_6F53A960(eventSlot, slot, &store, "CBattleNetEventFriendsStart", 0,0,0,0,0); break;
        case 15: BattleNetEventFriendsStop_Ctor_6F53AA50(eventSlot, slot, &store, "CBattleNetEventFriendsStop", 0,0,0,0,0); break;
        case 16: BattleNetEventFriendsAdd_Ctor_6F54A500(eventSlot, slot, &store, "CBattleNetEventFriendsAdd", 0,0,0,0,0); break;
        case 17: BattleNetEventFriendsRemove_Ctor_6F53AB40(eventSlot, slot, &store, "CBattleNetEventFriendsRemove", 0,0,0,0,0); break;
        case 18: BattleNetEventFriendsPosition_Ctor_6F53AC30(eventSlot, slot, &store, "CBattleNetEventFriendsPosition", 0,0,0,0,0); break;
        case 19: BattleNetEventFriendsUpdate_Ctor_6F547640(eventSlot, slot, &store, "CBattleNetEventFriendsUpdate", 0,0,0,0,0); break;
        case 20: BattleNetEventReadUserData_Ctor_6F54A630(eventSlot, slot, &store, "CBattleNetEventReadUserData", 0,0,0,0,0); break;
        case 21: BattleNetEventProfile_Ctor_6F548060(eventSlot, slot, &store, "CBattleNetEventProfile", 0,0,0,0,0); break;
        case 22: BattleNetEventClanProfile_Ctor_6F548190(eventSlot, slot, &store, "CBattleNetEventClanProfile", 0,0,0,0,0); break;
        case 23: BattleNetEventCheckDataFile_Ctor_6F53AD20(eventSlot, slot, &store, "CBattleNetEventCheckDataFile", 0,0,0,0,0); break;
        case 24: BattleNetEventEnterChat_Ctor_6F547760(eventSlot, slot, &store, "CBattleNetEventEnterChat", 0,0,0,0,0); break;
        case 25: BattleNetEventChannelList_Ctor_6F54B3F0(eventSlot, slot, &store, "CBattleNetEventChannelList", 0,0,0,0,0); break;
        case 26: BattleNetEventChatEvent_Ctor_6F547890(eventSlot, slot, &store, "CBattleNetEventChatEvent", 0,0,0,0,0); break;
        case 27: BattleNetEventMessageBox_Ctor_6F5479C0(eventSlot, slot, &store, "CBattleNetEventMessageBox", 0,0,0,0,0); break;
        case 34: BattleNetEventTeamGetPotentials_Ctor_6F54A850(eventSlot, slot, &store, "CBattleNetEventTeamGetPotentials", 0,0,0,0,0); break;
        case 39: BattleNetEventAnonProfile_Ctor_6F54C9A0(eventSlot, slot, &store, "CBattleNetEventAnonProfile", 0,0,0,0,0); break;
        case 40: BattleNetEventAnonProfileClan_Ctor_6F54A730(eventSlot, slot, &store, "CBattleNetEventAnonProfileClan", 0,0,0,0,0); break;
        case 44: BattleNetEventClanGetPotentials_Ctor_6F54A950(eventSlot, slot, &store, "CBattleNetEventClanGetPotentials", 0,0,0,0,0); break;
        case 45: BattleNetEventClanCreate_Ctor_6F547AF0(eventSlot, slot, &store, "CBattleNetEventClanCreate", 0,0,0,0,0); break;
        case 46: BattleNetEventClanFounder_Ctor_6F54AA50(eventSlot, slot, &store, "CBattleNetEventClanFounder", 0,0,0,0,0); break;
        case 47: BattleNetEventClanDisband_Ctor_6F53AE10(eventSlot, slot, &store, "CBattleNetEventClanDisband", 0,0,0,0,0); break;
        case 48: BattleNetEventClanDesignate_Ctor_6F53AF00(eventSlot, slot, &store, "CBattleNetEventClanDesignate", 0,0,0,0,0); break;
        case 49: BattleNetEventClanAdded_Ctor_6F53AFF0(eventSlot, slot, &store, "CBattleNetEventClanAdded", 0,0,0,0,0); break;
        case 50: BattleNetEventClanRemoved_Ctor_6F53B0E0(eventSlot, slot, &store, "CBattleNetEventClanRemoved", 0,0,0,0,0); break;
        case 51: BattleNetEventClanMemberAdd_Ctor_6F53B1D0(eventSlot, slot, &store, "CBattleNetEventClanMemberAdd", 0,0,0,0,0); break;
        case 52: BattleNetEventClanMemberRemove_Ctor_6F53B2C0(eventSlot, slot, &store, "CBattleNetEventClanMemberRemove", 0,0,0,0,0); break;
        case 53: BattleNetEventClanMemberInvite_Ctor_6F547C00(eventSlot, slot, &store, "CBattleNetEventClanMemberInvite", 0,0,0,0,0); break;
        case 54: BattleNetEventClanMemberRank_Ctor_6F53B3B0(eventSlot, slot, &store, "CBattleNetEventClanMemberRank", 0,0,0,0,0); break;
        case 55: BattleNetEventClanInfoGet_Ctor_6F547D20(eventSlot, slot, &store, "CBattleNetEventClanInfoGet", 0,0,0,0,0); break;
        case 56: BattleNetEventClanListStart_Ctor_6F53B4A0(eventSlot, slot, &store, "CBattleNetEventClanListStart", 0,0,0,0,0); break;
        case 57: BattleNetEventClanListStop_Ctor_6F53B590(eventSlot, slot, &store, "CBattleNetEventClanListStop", 0,0,0,0,0); break;
        case 58: BattleNetEventClanListRemove_Ctor_6F547E40(eventSlot, slot, &store, "CBattleNetEventClanListRemove", 0,0,0,0,0); break;
        case 59: BattleNetEventClanListUpdate_Ctor_6F54ABA0(eventSlot, slot, &store, "CBattleNetEventClanListUpdate", 0,0,0,0,0); break;
        case 60: BattleNetEventClanChangeRank_Ctor_6F547F50(eventSlot, slot, &store, "CBattleNetEventClanChangeRank", 0,0,0,0,0); break;
        case 61: BattleNetEventTournamentInfo_Ctor_6F54B4F0(eventSlot, slot, &store, "CBattleNetEventTournamentInfo", 0,0,0,0,0); break;
        case 62: BattleNetEventScheduledGame_Ctor_6F54B5F0(eventSlot, slot, &store, "CBattleNetEventScheduledGame", 0,0,0,0,0); break;
        case 63: BattleNetEventMatchmakingInfo_Ctor_6F53B680(eventSlot, slot, &store, "CBattleNetEventMatchmakingInfo", 0,0,0,0,0); break;
        case 64: BattleNetEventIconList_Ctor_6F54ACD0(eventSlot, slot, &store, "CBattleNetEventIconList", 0,0,0,0,0); break;

        case 43:
        {
            //  --------------------------------------------------------------
            //  Raw type 0x6B ("download file") - the one non-uniform table
            //  entry.  Best-effort translation of the dump's own instructions
            //  (see the two thunked leaves above); flagged DIFFERS, not a
            //  proven reconstruction of this case's real semantics.
            //  --------------------------------------------------------------
            unsigned char localWireIdBuf[0x14];
            ConstructDownloadFileWireIdBuf_6F544AF0(localWireIdBuf, 0x400900E2u);

            unsigned int remoteFileId = 0;
            ReadDownloadFileRecord_6F6878D0(&store, &remoteFileId);

            if ((unsigned int)store.m_readPos > (unsigned int)store.m_field10)
            {
                CNetData_ParseError("BattleNet::EVENT_ID_DOWNLOADFILE");
            }
            else
            {
                ((CNetPlayerRecord*)eventSlot)->FireToObserverLists(
                    (CNetCommandBase*)localWireIdBuf);
            }
            (void)remoteFileId;
            break;
        }

        default:
            break;
        }
    }

    //  Shared tail - every case above, the unhandled-index default, and the
    //  special case all converge here.  The dump builds a base
    //  ??_7TRefCnt@@6B@ local and, unless the record's own extra-data flag
    //  was never latched, hands it to the shared recycle pointer
    //  (off_6F932C0C / g_pfnRecycleBuffer, already declared in
    //  Net/netevent_dispatch.cpp and cdatastore.h's own header comment)
    //  alongside the store and the raw record pointers - **not
    //  reconstructed here**: TRefCnt itself has no C++ reconstruction in
    //  this repo yet (the same reason SNetEventSaveComplete, netcommand_
    //  dispatch.cpp, carries a vtable pointer as a plain field rather than
    //  a real base), and guessing at the condition that gates the recycle
    //  call risked corrupting a pool a lot of other reconstructions still
    //  share. Named explicitly in the DIFFERS reason rather than silently
    //  dropped.
    (void)store;
    return -1;
}
