//============================================================================
//  CNetData::DispatchActionByte (0x6F550730) and
//  CNetData::DispatchQueuedTurnRecord (0x6F5516E0, formerly thunked as
//  CNetData_ParseTurnCommandStream in pausecallchainthunks.cpp - see the
//  rename note there and in docs/targets/CNetData_DispatchActionByte.md).
//
//  DispatchActionByte is the turn-stream action-byte dispatch root
//  docs/targets/PauseGame_LAN_networking.md already named without a body,
//  and the function Net wave 13's live stack (pausecallchainthunks.cpp)
//  confirmed is what actually calls CNetCommandPause_BuildAndFire.  See
//  docs/targets/CNetData_DispatchActionByte.md for the full instruction-
//  level walkthrough this file's shape is taken from: the SEH-frame gap
//  netcommand.h's own header comment already accepts for every sibling
//  builder in this module, the two-jump-table decode
//  (byte_6F55164C + jpt_6F550921 for the 133 command cases,
//  byte_6F551518 + jpt_6F5508D8 for the 30-case cheat-permission gate),
//  and the per-case calling convention (`player` in ecx = the per-player
//  CNetPlayerRecord, i.e. `this session record + index*0x304 + 8`; `ctx`
//  in edx = the player index; seven stack arguments - reader, sender,
//  commandName, suppressFire, replayFlag, turn, an always-zero trailer -
//  every sibling CNetCommandXxx_BuildAndFire in this module already
//  shares).
//
//  Of the 133 dispatch cases, 67 call an already-reconstructed
//  CNetCommandXxx_BuildAndFire (netcommand_pause.cpp,
//  netcommand_action_builders*.cpp, netcommand_unitorder_builders.cpp) -
//  including CNetCommandPlayerChatMatch (an earlier session) and, as of
//  this session, CNetCommandUnitSelectionModify and
//  CNetCommandUnitDefineControlGroup, both real below.  Eight more (the
//  four-times-two CNetCommandSyncStoreXxx/CNetCommandSyncClearXxx family -
//  Integer/Real/Boolean/Unit) are still naked thunks below; see their own
//  comment for why - each embeds a large (~0x300-0x330 byte) reserved
//  region with its own non-trivial member destructor called by address
//  (0x6F3EBE80 and seven siblings, SyncStoreUnit's own nested
//  0x6F4413B0/0x6F441150 ctor/dtor pair on top), and pinning the exact
//  byte layout of that region from this dump alone - with no dump of any
//  of those eight destructors or SyncStoreUnit's constructor to confirm
//  it against - was judged not worth the guess this session's budget
//  allowed; see docs/notes for what would settle it (an ida_query dump of
//  each).  Case 6 and cases 129-131 are special-cased inline in
//  the shipped body (no CNetCommandXxx builder call at all) and are
//  reproduced that way here too.  A handful of small leaf helpers this
//  function calls directly (the per-case record-header reader family, the
//  cheat-permission gate, the "last dispatched action" cache writer, the
//  two replay-data commands' own Attach hooks, and CNetCommandSave's own
//  constructor) are real reconstructions as of this session (see their
//  own comments below); CNetData_SubmitReplayPayload alone stays a
//  correctly-ABI'd naked thunk - its own dump (74 instructions) reaches
//  five more undumped callees and an indirect vtable-slot call, well
//  past a "small leaf" despite this file's own comment once calling it
//  one.
//============================================================================
#include "netcommand.h"
#include "cdatastore.h"
#include "cdatastorescratch.h"   // CDataStoreScratch - NetMsgWriteDword_6F537820's own param type
#include "gameconfig.h"   // g_unk6FAB65F4
#include "storm.h"        // SMemFree (Storm_403), used by the two array dtors below
#include "CFloat.h"       // CNetCommandSyncStoreReal's own value field
#include "nethandlepairarray.h"  // the two Attach hooks' own array header
#include "gamecontext.h"  // GetThreadLocalSlot/GameDataContextChecksumProviderHash - CNetData_FireTickNotification
#include "game.h"         // g_pTimeSync - CNetData_FireTickNotification
#include "residuekey.h"   // SResidueKey - CNetData_FoldNetChecksumLegs' own value wrapper

//  0x6F6EED50 - already reconstructed as ReadCFloat (cdatastore_readcfloat.cpp).
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

//----------------------------------------------------------------------------
//  Already-reconstructed CNetCommandXxx builders this dispatcher calls
//  into (64 of the 133 cases) - see netcommand_pause.cpp,
//  netcommand_action_builders*.cpp and netcommand_unitorder_builders.cpp
//  for their real bodies.  Re-declared here rather than pulled through a
//  shared header because none of those files declare one; every signature
//  below must mangle identically to the real definition or link_check.py
//  catches it as two-symbols-for-one-address.
//----------------------------------------------------------------------------
void __fastcall CNetCommandAllyPing_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandArrowKey_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandCheatCooldown_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandCheatCritter_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandCheatDefeat_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandCheatFastBuild_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandCheatFastDeath_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandCheatFood_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandCheatGod_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandCheatGold_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandCheatLumber_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandCheatMana_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandCheatNoDefeat_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandCheatNoVictory_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandCheatResearch_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandCheatResources_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandCheatSetTOD_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandCheatShowMap_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandCheatTechTree_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandCheatUpgrade_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandCheatVictory_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandDialogButtonClick_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandDialogClick_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandGameSpeedDecrement_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandGameSpeedIncrement_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandGameSpeed_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandPause_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandPlayerChatMatch_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandPlayerEndCinematic_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandReplayViewSpeedDecrement_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandReplayViewSpeedIncrement_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandResumeTriggerExec_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandReviveCancel_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandSaveDone_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandSelectableSelectionModify_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandShowSkill_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandShowSubmenu_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandSyncClearBoolean_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandSyncClearInteger_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandSyncClearReal_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandSyncClearUnit_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandSyncStoreBoolean_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandSyncStoreInteger_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandSyncStoreReal_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandSyncStoreUnit_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandTeamChangeAlliance_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandTeamTradeResources_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandTrackableHit_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandTrackableTrack_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandTrainCancel_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandTriggerSyncReady_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandUnitOrderBasic_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandUnitOrderTargetImage2_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandUnitOrderTargetImageFogged2_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandUnitOrderTargetImageFogged_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandUnitOrderTargetImage_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandUnitOrderTargetPoint_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandUnitRefreshSubGroup_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandUnitSelectControlGroup_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandUnitSelectSubGroup_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandUnitSelectionEvent_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);
void __fastcall CNetCommandUnpause_BuildAndFire(CNetPlayerRecord*, int, CDataStore*, unsigned char, const char*, int, int, unsigned int, int);

//----------------------------------------------------------------------------
//  CNetCommandUnitSelectionModify / CNetCommandUnitDefineControlGroup
//  (0x6F546370 / 0x6F546450, cases 22/23, wire ids 0xA0016/0xA0017) - the
//  same CNetCommandXxx_BuildAndFire shape every sibling in this module
//  uses, now real: the two fields beyond CNetCommandBase are a single
//  reserved byte at +0x18 and a three-dword TSGrowableArray-of-handle-pairs
//  header at +0x1C (field/m_count/m_data, matching the layout
//  sub_6F2CEC00 - an ida_query dump of it, not part of this batch's own
//  worklist, confirmed reads/writes esi+0/+4/+8 off exactly this offset)
//  and freed inline in the epilogue when non-null, with no out-of-line
//  member destructor call - unlike the eight Sync* classes below, whose
//  own reserved region is an order of magnitude bigger and whose
//  destructor genuinely is an out-of-line call this dump alone cannot
//  place a byte inside.  `sub esp, 28h` here matches sizeof(cmd)==0x28
//  exactly (0x18 base+pad, +1 reserved byte, +3 reserved bytes pad,
//  +0xC array header).  Each has its own per-TU duplicate of
//  CNetData::FireCommand (0x6F53BA90/0x6F53BAD0 - netcommand_fire.cpp,
//  the /OPT:ICF-less duplication docs/msvc-vc8-idioms.md already
//  documents) and its own Attach hook (0x6F554430/0x6F5544A0, still a
//  correctly-ABI'd naked thunk - not part of this batch's own worklist).
//----------------------------------------------------------------------------
class CNetCommandUnitSelectionModify : public CNetCommandBase
{
public:
    CNetCommandUnitSelectionModify(int ctx, unsigned char sender)
        : CNetCommandBase(0xA0016, 0x16)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_reserved18 = 0;
        m_arrayField0 = 0;
        m_arrayCount = 0;
        m_arrayData = 0;
    }
    ~CNetCommandUnitSelectionModify()
    {
        if (m_arrayData != 0)
            SMemFree(m_arrayData, ".?AUNETTAG@@", -2, 0);
    }

    unsigned char m_reserved18;   // +0x18
    unsigned char m_pad19[3];     // +0x19 (never touched)
    int           m_arrayField0;  // +0x1C
    int           m_arrayCount;   // +0x20
    void*         m_arrayData;    // +0x24
};

class CNetCommandUnitDefineControlGroup : public CNetCommandBase
{
public:
    CNetCommandUnitDefineControlGroup(int ctx, unsigned char sender)
        : CNetCommandBase(0xA0017, 0x17)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_reserved18 = 0;
        m_arrayField0 = 0;
        m_arrayCount = 0;
        m_arrayData = 0;
    }
    ~CNetCommandUnitDefineControlGroup()
    {
        if (m_arrayData != 0)
            SMemFree(m_arrayData, ".?AUNETTAG@@", -2, 0);
    }

    unsigned char m_reserved18;
    unsigned char m_pad19[3];
    int           m_arrayField0;
    int           m_arrayCount;
    void*         m_arrayData;
};

//  0x6F554430 / 0x6F5544A0 - per-class Attach hooks (fill the array from
//  the wire): a reserved byte, a wire-format word count, then that many
//  8-byte (int,int) pairs, each pair's two dwords read individually
//  through CDataStore::ReadDword in a self-returning chain (the same
//  "returns `this`" idiom NetMsgReadWord_6F6516C0's own comment above
//  documents).  The array header lives at `cmd+0x1C`
//  (m_arrayField0/m_arrayCount/m_arrayData) - exactly the field layout
//  NetHandlePairArray (nethandlepairarray.h) reconstructs from
//  sub_6F2CEC00/sub_6F2C9700 (SetCount/SetAlloc), an ida_query dump of
//  both not part of this batch's own worklist but needed as this
//  function's own callees.  `retn` bare - __fastcall(ecx=reader,
//  edx=cmd), returning the reader unchanged.
void* __fastcall CNetCommandUnitSelectionModify_Attach(CDataStore* reader, CNetCommandUnitSelectionModify* cmd)
{
    unsigned short wireCount;
    reader->ReadByte(&cmd->m_reserved18);
    reader->ReadWord(&wireCount);

    unsigned int count = wireCount;
    NetHandlePairArray* arr = (NetHandlePairArray*)&cmd->m_arrayField0;
    arr->SetCount(count);

    for (unsigned int i = 0; i < count; i++)
    {
        unsigned int* pair = (unsigned int*)cmd->m_arrayData + i * 2;
        CDataStore* store = reader->ReadDword(pair);
        pair++;
        store->ReadDword(pair);
    }

    return reader;
}
void* __fastcall CNetCommandUnitDefineControlGroup_Attach(CDataStore* reader, CNetCommandUnitDefineControlGroup* cmd)
{
    unsigned short wireCount;
    reader->ReadByte(&cmd->m_reserved18);
    reader->ReadWord(&wireCount);

    unsigned int count = wireCount;
    NetHandlePairArray* arr = (NetHandlePairArray*)&cmd->m_arrayField0;
    arr->SetCount(count);

    for (unsigned int i = 0; i < count; i++)
    {
        unsigned int* pair = (unsigned int*)cmd->m_arrayData + i * 2;
        CDataStore* store = reader->ReadDword(pair);
        pair++;
        store->ReadDword(pair);
    }

    return reader;
}

void __fastcall CNetCommandUnitSelectionModify_FireCommand(CNetPlayerRecord*, CNetCommandBase*, const char*, int, unsigned int);
void __fastcall CNetCommandUnitDefineControlGroup_FireCommand(CNetPlayerRecord*, CNetCommandBase*, const char*, int, unsigned int);

void __fastcall CNetCommandUnitSelectionModify_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandUnitSelectionModify cmd(ctx, sender);
    CNetCommandUnitSelectionModify_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetCommandUnitSelectionModify_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

void __fastcall CNetCommandUnitDefineControlGroup_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandUnitDefineControlGroup cmd(ctx, sender);
    CNetCommandUnitDefineControlGroup_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetCommandUnitDefineControlGroup_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  CNetCommandSyncStoreInteger/Real/Boolean and CNetCommandSyncClear
//  Integer/Real/Boolean/Unit (cases 107-109, 112-115) - seven of the eight
//  builders the earlier session's own comment above once left as naked
//  thunks, now real.  What actually unblocked them was the second
//  ida_query pass this session ran: sub_6F555080 (the shared "read three
//  wire-key strings" Attach helper all eight call first) and each
//  class's own Attach/`FireCommand` duplicate pair (0x6F5551xx/0x6F53C4xx
//  for the four Store builders, 0x6F5552xx/0x6F53C5xx for the four Clear
//  ones) all had real dump bodies waiting in asm/ - the earlier session
//  just hadn't looked past the builder's own body for them.
//
//  Layout, from the Store builders' own dumps (identical for all three -
//  Integer/Real/Boolean only differ in wire id/subIndex and the value's
//  type/reader):
//      CNetCommandBase                     +0x00 (0x18 bytes with padding)
//      char    m_key1[0x100]                +0x18
//      char    m_key2[0x100]                +0x118
//      char    m_key3[0x100]                +0x218
//      SSyncValueTail m_tail                +0x318 (see tslinkpairrelease.cpp)
//      <value>                              +0x330
//  confirmed three ways: the ctor's own explicit stores (zeroing every
//  field this class touches - each key's first byte, and every dword of
//  m_tail except its own leading, never-written discriminator), the
//  exception-unwind funclet's `lea ecx, [esi+318h]` (the *persistent*
//  object's own tail offset, matching the local temp's own arithmetic
//  exactly), and the Attach helper's own `ReadDword`/`ReadCFloat` call
//  targeting `cmd+0x330` right after m_tail.  The Clear builders share
//  the same three-string-buffer prefix but have no tail at all (no large
//  reserved region, no destructor call anywhere in their own dump,
//  confirmed by the total absence of any `call sub_6F3EBxxx`-shaped
//  instruction in any of the four) - their own exception-unwind funclet
//  target (0x6F43D460 and three siblings) is a full, separate destructor
//  this batch never needed to dump, because it only runs on the
//  `__except_handler4`-shaped SEH path this toolchain's fixed /GS- /EHs-c-
//  cannot reproduce (netcommand.h's own accepted gap, same as every other
//  sibling builder in this module).
//
//  The tail's own release call sits at a single point every path
//  converges on before return - even the `suppressFire` branch skips
//  straight to it rather than returning early, unlike every simpler
//  sibling builder in this module - which is why it is spelled as one
//  plain function call at the end of the body below rather than folded
//  into an automatic C++ destructor (which would also have to reproduce
//  a virtual-dispatch-safe "deleting destructor" this dump's own normal
//  flow never calls - only the unwind funclet does, and that funclet is
//  the same unreproducible SEH path above).
//
//      case 107 0x6F54BE30  CNetCommandSyncStoreInteger       (0xA006B)
//      case 108 0x6F54BF60  CNetCommandSyncStoreReal          (0xA006C)
//      case 109 0x6F54C090  CNetCommandSyncStoreBoolean       (0xA006D)
//      case 112 0x6F542B60  CNetCommandSyncClearInteger       (0xA0070)
//      case 113 0x6F542C60  CNetCommandSyncClearReal          (0xA0071)
//      case 114 0x6F542D60  CNetCommandSyncClearBoolean       (0xA0072)
//      case 115 0x6F542E60  CNetCommandSyncClearUnit          (0xA0073)
//
//  Case 110 (0x6F54C1C0, CNetCommandSyncStoreUnit, 0xA006E) stays a
//  correctly-ABI'd naked thunk: its own ctor (0x6F4413B0) does not call
//  CNetCommandBase's own inline pattern at all - it stamps the same three
//  key buffers but then constructs a *second*, much larger embedded
//  object at +0x340 through an out-of-line ctor (sub_6F098080, its own
//  dump now in asm/) that pre-fills a 2-element array with the encoded
//  bit pattern of `g_CFloatZero` (dword_6FAAE470, CFloat.h) rather than a
//  plain zero - a real embedded array type this batch did not chase past
//  its own layout.
//----------------------------------------------------------------------------
extern void __fastcall SyncValueTail_Release_6F3EBE80(void*);
extern void __fastcall SyncValueTail_Release_6F3EBF10(void*);
extern void __fastcall SyncValueTail_Release_6F3EBFA0(void*);

//  0x6F3EBE80/F10/FA0's own field, embedded at +0x318 of each Store
//  command below - see tslinkpairrelease.cpp for the destructor.
struct SSyncValueTail
{
    int          m_discriminator;   // +0x00 - never written by any ctor below
    TSLink<void> m_link1;           // +0x04
    TSLink<void> m_link2;           // +0x0C
    void*        m_ownedPtr;        // +0x14
};

class CNetCommandSyncStoreInteger : public CNetCommandBase
{
public:
    CNetCommandSyncStoreInteger(int ctx, unsigned char sender)
        : CNetCommandBase(0xA006B, 0x6B)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_key1[0] = 0;
        m_key2[0] = 0;
        m_key3[0] = 0;
    }

    char            m_key1[0x100];   // +0x18
    char            m_key2[0x100];   // +0x118
    char            m_key3[0x100];   // +0x218
    SSyncValueTail  m_tail;          // +0x318
    int             m_value;         // +0x330 - filled by Attach's ReadDword
};

class CNetCommandSyncStoreReal : public CNetCommandBase
{
public:
    CNetCommandSyncStoreReal(int ctx, unsigned char sender)
        : CNetCommandBase(0xA006C, 0x6C)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_key1[0] = 0;
        m_key2[0] = 0;
        m_key3[0] = 0;
    }

    char            m_key1[0x100];   // +0x18
    char            m_key2[0x100];   // +0x118
    char            m_key3[0x100];   // +0x218
    SSyncValueTail  m_tail;          // +0x318
    CFloat          m_value;         // +0x330 - filled by Attach's ReadCFloat
};

class CNetCommandSyncStoreBoolean : public CNetCommandBase
{
public:
    CNetCommandSyncStoreBoolean(int ctx, unsigned char sender)
        : CNetCommandBase(0xA006D, 0x6D)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_key1[0] = 0;
        m_key2[0] = 0;
        m_key3[0] = 0;
    }

    char            m_key1[0x100];   // +0x18
    char            m_key2[0x100];   // +0x118
    char            m_key3[0x100];   // +0x218
    SSyncValueTail  m_tail;          // +0x318
    int             m_value;         // +0x330 - filled by Attach's ReadDword
};

class CNetCommandSyncClearInteger : public CNetCommandBase
{
public:
    CNetCommandSyncClearInteger(int ctx, unsigned char sender)
        : CNetCommandBase(0xA0070, 0x70)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_key1[0] = 0;
        m_key2[0] = 0;
        m_key3[0] = 0;
    }

    char m_key1[0x100];   // +0x18
    char m_key2[0x100];   // +0x118
    char m_key3[0x100];   // +0x218
};

class CNetCommandSyncClearReal : public CNetCommandBase
{
public:
    CNetCommandSyncClearReal(int ctx, unsigned char sender)
        : CNetCommandBase(0xA0071, 0x71)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_key1[0] = 0;
        m_key2[0] = 0;
        m_key3[0] = 0;
    }

    char m_key1[0x100];   // +0x18
    char m_key2[0x100];   // +0x118
    char m_key3[0x100];   // +0x218
};

class CNetCommandSyncClearBoolean : public CNetCommandBase
{
public:
    CNetCommandSyncClearBoolean(int ctx, unsigned char sender)
        : CNetCommandBase(0xA0072, 0x72)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_key1[0] = 0;
        m_key2[0] = 0;
        m_key3[0] = 0;
    }

    char m_key1[0x100];   // +0x18
    char m_key2[0x100];   // +0x118
    char m_key3[0x100];   // +0x218
};

class CNetCommandSyncClearUnit : public CNetCommandBase
{
public:
    CNetCommandSyncClearUnit(int ctx, unsigned char sender)
        : CNetCommandBase(0xA0073, 0x73)
    {
        m_ctx = ctx;
        m_sender = sender;
        m_key1[0] = 0;
        m_key2[0] = 0;
        m_key3[0] = 0;
    }

    char m_key1[0x100];   // +0x18
    char m_key2[0x100];   // +0x118
    char m_key3[0x100];   // +0x218
};

//  0x6F555080 - the shared "read the three wire-key strings" helper every
//  one of the seven Attach hooks below calls first.  Three back-to-back
//  CDataStore::ReadString calls at the class's own +0x18/+0x118/+0x218
//  buffers; the follow-up check after each one (force-null the buffer's
//  first byte when the reader's own position has run past its declared
//  length) duplicates a check ReadString's own body already makes
//  internally - redundant-looking, but the shipped code does it at both
//  levels, so this does too.  `retn` bare - __fastcall(reader, cmdBase).
CDataStore* __fastcall CNetCommandSyncKeys_Read(CDataStore* reader, void* cmdBase)
{
    char* key;

    key = (char*)cmdBase + 0x18;
    reader->ReadString(key, 0x100);
    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
        *key = 0;

    key = (char*)cmdBase + 0x118;
    reader->ReadString(key, 0x100);
    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
        *key = 0;

    key = (char*)cmdBase + 0x218;
    reader->ReadString(key, 0x100);
    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
        *key = 0;

    return reader;
}

//  Attach hooks - each reads the three wire-key strings above, then, for
//  the three Store builders only, the value itself at +0x330 (ReadDword
//  for Integer/Boolean, ReadCFloat for Real - cdatastore.h's existing
//  signatures).  `retn` bare on every one, confirmed against each one's
//  own dump body - __fastcall(reader, cmd).
void* __fastcall CNetCommandSyncStoreInteger_Attach(CDataStore* reader, CNetCommandSyncStoreInteger* cmd)
{
    CNetCommandSyncKeys_Read(reader, cmd);
    reader->ReadDword((unsigned int*)&cmd->m_value);
    return reader;
}

void* __fastcall CNetCommandSyncStoreReal_Attach(CDataStore* reader, CNetCommandSyncStoreReal* cmd)
{
    CNetCommandSyncKeys_Read(reader, cmd);
    ReadCFloat(reader, &cmd->m_value);
    return reader;
}

void* __fastcall CNetCommandSyncStoreBoolean_Attach(CDataStore* reader, CNetCommandSyncStoreBoolean* cmd)
{
    CNetCommandSyncKeys_Read(reader, cmd);
    reader->ReadDword((unsigned int*)&cmd->m_value);
    return reader;
}

void* __fastcall CNetCommandSyncClearInteger_Attach(CDataStore* reader, CNetCommandSyncClearInteger* cmd)
{
    CNetCommandSyncKeys_Read(reader, cmd);
    return reader;
}

void* __fastcall CNetCommandSyncClearReal_Attach(CDataStore* reader, CNetCommandSyncClearReal* cmd)
{
    CNetCommandSyncKeys_Read(reader, cmd);
    return reader;
}

void* __fastcall CNetCommandSyncClearBoolean_Attach(CDataStore* reader, CNetCommandSyncClearBoolean* cmd)
{
    CNetCommandSyncKeys_Read(reader, cmd);
    return reader;
}

void* __fastcall CNetCommandSyncClearUnit_Attach(CDataStore* reader, CNetCommandSyncClearUnit* cmd)
{
    CNetCommandSyncKeys_Read(reader, cmd);
    return reader;
}

#undef SYNC_ATTACH_THUNK

//  0x6F53C410/450/490/510/550/590/5D0 - seven more byte-identical copies
//  of CNetData::FireCommand (netcommand_fire.cpp already has the body and
//  four other duplicates of it; this image was linked without /OPT:ICF -
//  see that file's own header comment), one per builder below.
void __fastcall CNetCommandSyncStoreInteger_FireCommand(CNetPlayerRecord* player, CNetCommandBase* cmd,
                                                          const char* name, int verbose, unsigned int turn);
void __fastcall CNetCommandSyncStoreReal_FireCommand(CNetPlayerRecord* player, CNetCommandBase* cmd,
                                                       const char* name, int verbose, unsigned int turn);
void __fastcall CNetCommandSyncStoreBoolean_FireCommand(CNetPlayerRecord* player, CNetCommandBase* cmd,
                                                          const char* name, int verbose, unsigned int turn);
void __fastcall CNetCommandSyncClearInteger_FireCommand(CNetPlayerRecord* player, CNetCommandBase* cmd,
                                                          const char* name, int verbose, unsigned int turn);
void __fastcall CNetCommandSyncClearReal_FireCommand(CNetPlayerRecord* player, CNetCommandBase* cmd,
                                                       const char* name, int verbose, unsigned int turn);
void __fastcall CNetCommandSyncClearBoolean_FireCommand(CNetPlayerRecord* player, CNetCommandBase* cmd,
                                                          const char* name, int verbose, unsigned int turn);
void __fastcall CNetCommandSyncClearUnit_FireCommand(CNetPlayerRecord* player, CNetCommandBase* cmd,
                                                       const char* name, int verbose, unsigned int turn);

void __fastcall CNetCommandSyncStoreInteger_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandSyncStoreInteger cmd(ctx, sender);
    CNetCommandSyncStoreInteger_Attach(reader, &cmd);

    if (!suppressFire)
    {
        if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
            CNetData_ParseError(commandName);
        else
            CNetCommandSyncStoreInteger_FireCommand(player, &cmd, commandName, replayFlag, turn);
    }

    SyncValueTail_Release_6F3EBE80(&cmd.m_tail);
}

void __fastcall CNetCommandSyncStoreReal_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandSyncStoreReal cmd(ctx, sender);
    CNetCommandSyncStoreReal_Attach(reader, &cmd);

    if (!suppressFire)
    {
        if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
            CNetData_ParseError(commandName);
        else
            CNetCommandSyncStoreReal_FireCommand(player, &cmd, commandName, replayFlag, turn);
    }

    SyncValueTail_Release_6F3EBF10(&cmd.m_tail);
}

void __fastcall CNetCommandSyncStoreBoolean_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandSyncStoreBoolean cmd(ctx, sender);
    CNetCommandSyncStoreBoolean_Attach(reader, &cmd);

    if (!suppressFire)
    {
        if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
            CNetData_ParseError(commandName);
        else
            CNetCommandSyncStoreBoolean_FireCommand(player, &cmd, commandName, replayFlag, turn);
    }

    SyncValueTail_Release_6F3EBFA0(&cmd.m_tail);
}

void __fastcall CNetCommandSyncClearInteger_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandSyncClearInteger cmd(ctx, sender);
    CNetCommandSyncClearInteger_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetCommandSyncClearInteger_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

void __fastcall CNetCommandSyncClearReal_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandSyncClearReal cmd(ctx, sender);
    CNetCommandSyncClearReal_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetCommandSyncClearReal_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

void __fastcall CNetCommandSyncClearBoolean_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandSyncClearBoolean cmd(ctx, sender);
    CNetCommandSyncClearBoolean_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetCommandSyncClearBoolean_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

void __fastcall CNetCommandSyncClearUnit_BuildAndFire(
    CNetPlayerRecord* player, int ctx, CDataStore* reader, unsigned char sender,
    const char* commandName, int suppressFire, int replayFlag, unsigned int turn, int unused18)
{
    (void)unused18;

    CNetCommandSyncClearUnit cmd(ctx, sender);
    CNetCommandSyncClearUnit_Attach(reader, &cmd);

    if (suppressFire)
        return;

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
    {
        CNetData_ParseError(commandName);
        return;
    }

    CNetCommandSyncClearUnit_FireCommand(player, &cmd, commandName, replayFlag, turn);
}

//----------------------------------------------------------------------------
//  Case 110 (0x6F54C1C0, CNetCommandSyncStoreUnit, 0xA006E) - see the
//  header comment above the seven real reconstructions for why this one
//  alone is still a correctly-ABI'd naked thunk.  `retn 1Ch`, confirmed
//  against its own dump body.
//----------------------------------------------------------------------------
__declspec(naked) void __fastcall CNetCommandSyncStoreUnit_BuildAndFire(
    CNetPlayerRecord*, int, CDataStore*, unsigned char,
    const char*, int, int, unsigned int, int)
{
    __asm { mov eax, 06F54C1C0h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  Leaf helpers DispatchActionByte itself calls directly (not through a
//  CNetCommandXxx builder).  All but CNetData_SubmitReplayPayload (see its
//  own comment) are real reconstructions as of this session; each header
//  comment below still records the `retn`/register facts the earlier naked
//  thunk was built from, now folded into the real body.
//----------------------------------------------------------------------------
#define LEAF_THUNK(sig, addr)      \
    __declspec(naked) sig          \
    {                              \
        __asm { mov eax, addr }    \
        __asm { jmp eax }          \
    }

//  0x6F6516C0 - already reconstructed as NetMsgReadWord_6F6516C0
//  (netmsgio_651.cpp): read the two-byte record-count prefix off the outer
//  turn-stream CDataStore through the already-reconstructed
//  CDataStore::ReadWord, and return `this`.  `retn` bare -
//  __fastcall(self, dst) matches ReadWord's own real convention, a plain
//  __thiscall member (this=ecx, out on the stack): the caller passes
//  `dst` in edx purely because DispatchActionByte's own call site does,
//  not because this function forwards it as a register.  Redeclared here
//  for extern linkage rather than re-thunked under a second name - this
//  file used to carry its own duplicate body (CNetData_ReadRecordCount,
//  the "one shipped address, two names" defect
//  netevent_replaystream_records.cpp's own comment on this same address
//  already flagged as out of scope for that session's batch); collapsed to
//  the one real name when the merge that surfaced the collision needed a
//  resolution anyway.  Its own explicit-local-before-return shape (not
//  the tail `jmp` /Ob2 makes of `return self->ReadWord(dst);` written as
//  one expression, docs/msvc-vc8-idioms.md's "a tail jmp into a function
//  with the same argument list") lives with the real definition now.
extern CDataStore* __fastcall NetMsgReadWord_6F6516C0(CDataStore* self, unsigned short* dst);

//  0x6F537B10 - already reconstructed (trivialaccessors_05.cpp): "store 1
//  into the dword at +0xAD0".  Case 131's own real tail call (see the
//  case body below) - not a CNetCommandXxx fire at all.
void __fastcall SetField0xAD0_6F537B10(void* self);

//  0x6F651AB0 - CNetData_ReadSenderKeyAndLength's own helper (cap 0x400) -
//  own translation unit (netcommand_readlengthprefixed.cpp) so its one
//  call site does not let /Ob2 fold it back in: the real dump has a
//  genuine `call` to a separate address here, and an inlined copy would
//  reproduce the *bytes* read but not the *shape* (one shared out-of-line
//  function vs. its logic folded into the caller).  See that file for the
//  body and its own header comment (shared with ReadReplayPayloadHeader,
//  0x6F5545F0, the cap-0x3FD twin further below).
CDataStore* __fastcall ReadLengthPrefixedPayload(CDataStore* self, unsigned char* dst);

//  0x6F652300 - read the per-sub-record 1-byte "sender key" through
//  ReadByte, then the 2-byte length prefix plus its raw payload through
//  ReadLengthPrefixedPayload above (cap 0x400) into `dst[1..]` - so
//  `dst[0]` is the sender key, `dst[1..2]` the length, and `dst[3..]` the
//  payload, matching the "whole sub-record" comment on this call site in
//  DispatchActionByte below.  `retn` bare - __fastcall(self, dst).
CDataStore* __fastcall CNetData_ReadSenderKeyAndLength(CDataStore* self, unsigned char* dst)
{
    CDataStore* store = self;
    store->ReadByte(dst);
    ReadLengthPrefixedPayload(store, dst + 1);
    return store;
}

//  0x6F54E7C0 - not part of this batch's own worklist (a `TAllocatorHashTable`
//  -shaped bucket walk over a global table at `unk_6FACC8BC`, keyed by the
//  raw wire byte - sub_6F54C870, one level deeper still); left as a
//  correctly-ABI'd naked thunk.  `retn 4` - __fastcall(ecx=slotTable,
//  key on the stack).  `edx` is dead inside this function's own body and
//  the real call site never sets it either - MSVC has no free-function
//  calling convention that uses `ecx` alone without also being a member
//  (`__thiscall` on a free function is C3865, and a naked member function
//  is its own rejection, C2488), so this declaration's spurious `edx`
//  parameter costs one `xor edx,edx` at the one call site below that a
//  real member spelling would not - the only register-materialisation gap
//  left in this file's own real reconstructions.  `ecx` still has to be
//  threaded through explicitly: the shipped chain (that caller -> this
//  function -> sub_6F54C870) relies on `ecx` surviving untouched across
//  both calls rather than being reloaded, so a C++ spelling that does not
//  also pass it explicitly would let the compiler treat it as unused and
//  drop the real lookup's own `this`.
LEAF_THUNK(void* __fastcall CNetData_SenderFromKeyLookup(void* slotTable, void* /*unused_edx*/, unsigned int key),
           0x6F54E7C0)

//  0x6F54E7E0 - CNetData_SenderFromKey's own real body: look the raw wire
//  "sender key" byte up via CNetData_SenderFromKeyLookup above, and read
//  the sender byte at the found entry's +0x36 - or 0xFF if the lookup
//  found nothing.  `retn 4` - __fastcall(ecx=slotTable, key on the
//  stack), forwarded to the lookup exactly as received.
unsigned char __fastcall CNetData_SenderFromKey(void* slotTable, void* /*unused_edx*/, unsigned int key)
{
    void* entry = CNetData_SenderFromKeyLookup(slotTable, 0, key);
    if (entry != 0)
        return *((unsigned char*)entry + 0x36);
    return 0xFF;
}

//  0x6F3A1650 - IndexedArrayHolder_6F3A1650::At, already reconstructed
//  under that name (src/Unit/unit_playertable.cpp) - re-declared here
//  through a raw function-pointer call below rather than a second
//  `extern` declaration, because that struct type is a TU-local detail of
//  unit_playertable.cpp and pulling in a second declaration risks a
//  signature drift link_check.py would (rightly) flag as two symbols for
//  one address.  Called by its real address directly.
typedef int (__thiscall *IndexedArrayHolderAtFn)(void* self, unsigned int index);
static const IndexedArrayHolderAtFn IndexedArrayHolder_At = (IndexedArrayHolderAtFn)0x6F3A1650;

//  0x6F473170 / 0x6F40B1C0 - already reconstructed
//  (HandleRefFieldOwner_6F473170::QueryField78 and GetField0x2A8_6F40B1C0
//  respectively, both first declared in the GameUI player-name/pause
//  family and re-declared locally here the way every other caller in this
//  repo does - no shared header for either).
struct HandleRefFieldOwner_6F473170 { int __thiscall QueryField78(); };
int __fastcall GetField0x2A8_6F40B1C0(const void* self);

//  0x6F545270 - CNetData::CheckCheatPermission.  Given the holder-array
//  entry for the raw wire action byte (or null) and the byte itself,
//  decides whether the corresponding CNetCommandXxx should have its own
//  fire suppressed (1) or not (0).  A null entry, or any action byte
//  outside [1,0x6A], is suppressed outright; an entry whose "cheat gate"
//  field (+0x270) is armed and whose embedded HandleRefFieldOwner
//  (+0xF0)'s own QueryField78 reads zero is allowed outright.  Otherwise
//  the raw byte drives IDA's own jump table (byte_6F5452DC/jpt_6F5452A7):
//  1/2/6 defer to GetField0x2A8_6F40B1C0 (suppressed only when it reads
//  zero); 98/104/105/106 are always allowed; every other byte in range is
//  suppressed.  `retn` bare - __fastcall(ecx=holderEntry, dl=byte, a
//  truncated edx register argument).
int __fastcall CNetData_CheckCheatPermission(void* holderEntry, unsigned char actionByte)
{
    if (holderEntry == 0)
        return 1;

    if (*(int*)((char*)holderEntry + 0x270) == 1 &&
        ((HandleRefFieldOwner_6F473170*)((char*)holderEntry + 0xF0))->QueryField78() == 0)
        return 0;

    if (actionByte < 1 || actionByte > 0x6A)
        return 1;

    switch (actionByte)
    {
    case 1: case 2: case 6:
        return (GetField0x2A8_6F40B1C0(holderEntry) == 0) ? 1 : 0;
    case 98: case 104: case 105: case 106:
        return 0;
    default:
        return 1;
    }
}

//  0x6F43D110 - CNetCommandSave's own constructor: builds the small
//  diagnostic block for the "no builder for this action byte" fallback
//  path (case 6's own "not case 6" branch).  A plain `__thiscall`
//  constructor (this=ecx, arg on the stack) - the real call site
//  (`push 6; lea ecx,[...]; call sub_6F43D110`, no edx set up at all)
//  shows the "unused edx" this session's earlier naked-thunk declaration
//  guessed at was never real.  Defined out-of-line below (not inline in
//  the class body) for the same reason CDataStoreCache1460::OnConstruct
//  is out-of-line in cdatastorecache.cpp: an in-class definition is a
//  strong inlining hint, and /Ob2 folded this one-call-site constructor
//  straight into CNetData_DispatchActionByte's own body - abi_audit.py's
//  own "WRONG CLASS" finding (DispatchActionByte stamping
//  ??_7CNetCommandSave@@6B@, which its own dump never shows) is exactly
//  that inlining, not a real defect in either function.
class CNetCommandSave : public CNetCommandBase
{
public:
    CNetCommandSave(unsigned char caseNum);
    unsigned char m_reserved18;   // +0x18
};

//  `caseNum` is a byte-wide parameter, not `int` - the target reads it
//  with `mov cl,[esp+4]` (a one-byte load), not `mov ecx,[esp+4]`.
CNetCommandSave::CNetCommandSave(unsigned char caseNum)
    : CNetCommandBase(0xA0000 | ((unsigned int)caseNum & 0xFFFF), caseNum)
{
    m_ctx = 2;
    m_reserved18 = 0;
}

//  0x6F5547C0 - CNetData_RememberLastAction's own real body: "remember
//  the last dispatched action" - reads a NUL-terminated string (the
//  matched command's own name, up to 0x104 bytes) off `reader` into
//  `dest+0x18` through the already-reconstructed CDataStore::ReadString,
//  then zeroes it if the read ran the stream past its declared length.
//  Called both from case 6's own special body (dest = self+0x1B48) and
//  from the fallback diagnostic path (dest = the freshly-built
//  CNetCommandSave above).  `retn` bare - __fastcall(ecx=reader,
//  edx=dest) - `this` is genuinely the reader, matching the earlier naked
//  thunk's own argument order (`dest`/`cmd` were just its two names for
//  the same two positions).
CDataStore* __fastcall CNetData_RememberLastAction(CDataStore* reader, void* dest)
{
    char* nameBuf = (char*)dest + 0x18;
    reader->ReadString(nameBuf, 0x104);

    if ((unsigned int)reader->m_readPos > (unsigned int)reader->m_field10)
        *nameBuf = 0;

    return reader;
}

//  0x6F5545F0 - the replay-payload twin of ReadLengthPrefixedPayload
//  above, cap 0x3FD rather than 0x400 (the only difference the dump
//  shows) - shared by both replay-data Attach hooks below.  Own
//  translation unit for the same /Ob2-folding reason (two call sites in
//  this file were still enough for the inliner to fold both copies in).
CDataStore* __fastcall ReadReplayPayloadHeader(CDataStore* self, unsigned char* dst);

//  0x6F554680 / 0x6F5546C0 - the CNetCommandReplayDataUncompressed /
//  CNetCommandReplayDataCompressed Attach hooks (cases 129/130's own
//  inline command construction - see the special-cased bodies below):
//  both just forward to ReadReplayPayloadHeader above at `cmd+0x18`
//  (`m_dataSize`) and return the reader.  `retn` bare -
//  __fastcall(ecx=reader, edx=cmd).
void* __fastcall CNetCommandReplayDataUncompressed_Attach(CDataStore* reader, void* cmd)
{
    //  `reader` returned from a local, not straight off the call - see
    //  NetMsgReadWord_6F6516C0's own comment above for why.
    CDataStore* store = reader;
    ReadReplayPayloadHeader(store, (unsigned char*)cmd + 0x18);
    return store;
}
void* __fastcall CNetCommandReplayDataCompressed_Attach(CDataStore* reader, void* cmd)
{
    CDataStore* store = reader;
    ReadReplayPayloadHeader(store, (unsigned char*)cmd + 0x18);
    return store;
}

//  0x6F5500E0 - hand the just-read replay-data payload (raw bytes + size)
//  to the outbound path once a ReplayDataUncompressed/Compressed/Done
//  command has actually fired.  `retn 0Ch` (__fastcall(self, src, size,
//  flag) - two stack arguments beyond the two registers).  Left as a
//  correctly-ABI'd naked thunk, not part of this batch's own worklist:
//  its own dump (74 instructions) reaches five further undumped callees
//  (sub_6F549450, sub_6F6563A0, sub_6F54C7E0, sub_6F4C25A0, sub_6F54EB00)
//  plus one indirect vtable-slot call - a "small leaf" this file's own
//  header comment once called it, wrongly; corrected above.
LEAF_THUNK(void __fastcall CNetData_SubmitReplayPayload(void* self, void* /*unused_edx*/,
                                                        void* src, unsigned int size, int flag),
           0x6F5500E0)

#undef LEAF_THUNK


//----------------------------------------------------------------------------
//  The three replay-data commands (cases 129-131) are constructed inline
//  by the dispatcher itself rather than through a shared _BuildAndFire
//  wrapper - each stamps a literal `m_ctx` (2/3/4, not the dispatcher's own
//  runtime player-index `ctx`) and carries one extra 2-byte payload-size
//  field their own Attach hook (thunked above) fills in.  See
//  docs/targets/CNetData_DispatchActionByte.md.
//----------------------------------------------------------------------------
class CNetCommandReplayDataUncompressed : public CNetCommandBase
{
public:
    CNetCommandReplayDataUncompressed()
        : CNetCommandBase(0xA0081, 0x81)
    {
        m_ctx = 2;
    }
    unsigned short m_dataSize;   // +0x18
};

class CNetCommandReplayDataCompressed : public CNetCommandBase
{
public:
    CNetCommandReplayDataCompressed()
        : CNetCommandBase(0xA0082, 0x82)
    {
        m_ctx = 3;
    }
    unsigned short m_dataSize;   // +0x18
};

class CNetCommandReplayDataDone : public CNetCommandBase
{
public:
    CNetCommandReplayDataDone()
        : CNetCommandBase(0xA0083, 0x83)
    {
        m_ctx = 4;
    }
};

//  0x6F53E510 - CNetData::NotifyResyncComplete.  Called once from
//  DispatchQueuedTurnRecord below when a player's own m_saveStillPending
//  flag (CNetData's own unshifted view, netdata.h - the same "index this
//  record, not the +8 view" convention that class documents) is set.
//  `this` is the whole session (SNetSessionInfo*, same as
//  DispatchQueuedTurnRecord's own), and `self+index*0x304` is a CNetData*
//  whose `Session()` is the CNetPlayerRecord already reconstructed.
//
//  If a save was still pending, the player is bounced back to PLAYING;
//  either way the pausing/resume-blocked bookkeeping and the
//  pending flag itself are cleared, and a `CNetEventSaveComplete` is fired
//  to both observer lists.  That event's own vtable is real but
//  unreconstructed (0x6F958108, `ida_query names`) - stamped as the real
//  shipped address rather than a from-scratch C++ class for the same
//  reason CTurnsSyncStore's own ctor does below (netclient.h): nothing
//  this closure reaches ever dispatches through it (FireToObserverLists
//  only reads the leading SAgentMessage::m_id field), so there is nothing
//  here to reconstruct, only a pointer to get right.
//----------------------------------------------------------------------------
struct SNetEventSaveComplete
{
    void* m_vtable;      // +0x00 - real ??_7CNetEventSaveComplete@@6B@ (0x6F958108)
    int   m_field4;       // +0x04 = 0
    int   m_id;            // +0x08 = 0x40090096 (SAgentMessage::m_id)
    int   m_field0xC;     // +0x0C = 0
    char  m_reserved10[4];
    int   m_zero;          // +0x14 = the always-0 argument this closure's own call site passes
};

const void* const kNetEventSaveCompleteVftable = (const void*)0x6F958108;
const int kNetEventSaveCompleteId = 0x40090096;

void __fastcall CNetData_NotifyResyncComplete(SNetSessionInfo* self, void* /*unused_edx*/,
                                    unsigned int playerIndex, int zero)
{
    //  `rec` stays the unshifted CNetData view throughout, matching the
    //  shipped body exactly - it only forms the +8 CNetPlayerRecord view
    //  at the two call sites below, rather than once up front the way
    //  every other reconstruction in this file reads a `player` pointer.
    CNetData* rec = (CNetData*)((char*)self + playerIndex * 0x304);

    if (rec->m_saveStillPending != 0)
        rec->Session()->SetGameState(kGameStatePlaying);

    rec->m_pausingSlot = 0xFF;
    rec->m_resumeBlocked = 0;
    rec->m_saveStillPending = 0;

    SNetEventSaveComplete evt;
    evt.m_vtable = (void*)kNetEventSaveCompleteVftable;
    evt.m_field4 = 0;
    evt.m_id = kNetEventSaveCompleteId;
    evt.m_field0xC = 0;
    evt.m_zero = zero;
    rec->Session()->FireToObserverLists((CNetCommandBase*)&evt);
}

//  g_unk6FAB778C - the "tempest presence" host: a global object whose
//  vtable-style array at +0x0C/+0x1C (already partially characterized
//  outside this session, see docs/notes/checksum-provider-registry.md)
//  TempestProviderListHash walks (tempestproviderlisthash.cpp), and whose
//  +0x40/+0x1C/+0x50 also feed CNetData::FireTickNotification's own
//  "birth tag"/"presence tag" legs directly below (not through the
//  provider-list walk at all).  Left opaque (raw offset casts, no named
//  struct) rather than typed as SNetSessionInfo/CNetPlayerRecord: this
//  session found no evidence either way for which class it really is,
//  only that it is a *different* object from `self` below (both are read
//  in the same function, at different addresses).
extern void* g_unk6FAB778C;

//  0x6F543D40 / 0x6F280BE0 - TempestProviderListHash
//  (tempestproviderlisthash.cpp) and CTimeSync_HashInto
//  (Sync/ctimesynchashinto.cpp) are declared, not defined, here on
//  purpose: both are real functions the shipped binary never inlines
//  into this file's own CNetData_FireTickNotification (every dump shows
//  a real `call`), but a *definition* visible in this same TU is a
//  standing invitation for this compiler's /Ob2 to auto-inline them here
//  anyway - which happened once, and cost FireTickNotification's own
//  listing an instruction (32/55 -> 31/55 against its committed report)
//  with no change to FireTickNotification's own source.  See each
//  function's own translation unit for why it is a `call` here rather
//  than a body.
unsigned int __fastcall TempestProviderListHash(void* presenceHost);
void __fastcall CTimeSync_HashInto(CTimeSync* self, unsigned int* accum);

//----------------------------------------------------------------------------
//  0x6F545840 - CNetData::FireTickNotification.  Builds the per-tick
//  "net" turn-sync value out of five independently-computed legs and
//  writes it to self+0x1C64 (m_tickChecksum below) - see
//  docs/notes/checksum-provider-registry.md for the whole mechanism this
//  is the live (non-dead-code) half of, and for why the other six
//  category names sub_6F5455A0 prints (cheat/combat/rand/unit/game, plus
//  this same "net" leg by a different name) are dead code that never
//  actually runs.
//
//  self+0x2F8/+0x1C64/+0x1C70 stay raw offset casts rather than named
//  SNetSessionInfo members, the same way CNetData_NotifyResyncComplete
//  above forms `self + index*0x304` without touching the shared struct:
//  which class actually owns them is not established by this function
//  alone (see the g_unk6FAB778C comment above for the same caution).
//----------------------------------------------------------------------------
void __fastcall CNetData_FireTickNotification(SNetSessionInfo* self)
{
    CGameDataContext* context =
        (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);

    //  The dump zeroes seven consecutive stack dwords up front (one
    //  aggregate zero-init) before either of the two hash walks runs;
    //  only five of the seven are ever read again below, but all seven
    //  get the store.
    struct
    {
        unsigned int netHash, tempest, birthTag, presenceTag, timeSyncHash;
        unsigned int unused1, unused2;
    } legs = { 0, 0, 0, 0, 0, 0, 0 };

    unsigned int netChecksum = GameDataContextChecksumProviderHash(context);

    void* host = g_unk6FAB778C;
    legs.netHash = netChecksum ^ 0x17ABu;
    legs.netHash = (legs.netHash << 3) | (legs.netHash >> 29);

    //  tempest defaults to the session's own last-reported value
    //  (self+0x1C70) and is only recomputed off the presence host when
    //  this session actually has game results queued (self+0x2F8 != 0).
    legs.tempest = *(unsigned int*)((char*)self + 0x1C70);
    if (*(unsigned int*)((char*)self + 0x2F8) != 0)
        legs.tempest = TempestProviderListHash(host);

    unsigned int fallbackOrHost = *(unsigned int*)((char*)host + 0x40);
    unsigned int birthTag = (fallbackOrHost == 0xFFFFFFFFu)
                                 ? *(unsigned int*)((char*)host + 0x1C)
                                 : fallbackOrHost;
    legs.presenceTag = *(unsigned int*)((char*)host + 0x50);
    legs.birthTag = birthTag;

    CTimeSync_HashInto(g_pTimeSync, &legs.timeSyncHash);

    *(unsigned int*)((char*)self + 0x1C64) = 0;

    unsigned int hash = legs.tempest ^ legs.netHash;
    hash = (hash << 3) | (hash >> 29);
    hash ^= legs.birthTag;
    hash = (hash << 3) | (hash >> 29);
    hash ^= legs.presenceTag;
    hash = (hash << 3) | (hash >> 29);
    hash ^= legs.timeSyncHash;
    hash = (hash << 3) | (hash >> 29);

    *(unsigned int*)((char*)self + 0x1C64) = hash;
}

//----------------------------------------------------------------------------
//  0x6F545910 - a thin wrapper that returns just the "tempest" leg
//  FireTickNotification above also computes: touch the per-thread
//  checksum-provider context (GetThreadLocalSlot(kThreadLocalGameData),
//  result unused here - the same "make sure the lazy-singleton context
//  exists" idiom this session's docs/notes/checksum-provider-registry.md
//  already documents elsewhere for this array) and tail-call
//  TempestProviderListHash on the shared presence host.  A genuine tail
//  call in the shipped body (`jmp sub_6F543D40`, not `call`+`retn`), which
//  is why the C++ below is written as a direct `return` of the call rather
//  than storing to a local first.
//----------------------------------------------------------------------------
unsigned int __fastcall TempestChecksumLegOnly()
{
    GetThreadLocalSlot(kThreadLocalGameData);
    return TempestProviderListHash(g_unk6FAB778C);
}

//----------------------------------------------------------------------------
//  0x6F545930 - a second, independent fold of the same five legs
//  FireTickNotification above computes (net/tempest/birthTag/presenceTag/
//  timeSync), returned rather than stored to self+0x1C64.  No static caller
//  found in this session's own worklist (see the write-up) - this is a
//  TODO caller of two already-`EXACT` checksum primitives
//  (GameDataContextChecksumProviderHash, TempestProviderListHash) reached
//  by address-neighbourhood search, not by an xref chain.
//
//  Unlike FireTickNotification's own xor/rol-3 fold, this one seeds the
//  accumulator directly with the raw net hash (a plain store, not an xor)
//  and folds every subsequent leg through the same `SResidueKey::Set` +
//  `CTimeSync_HashInto` byte-scramble primitive Sync/ctimesynchashinto.cpp
//  already reconstructs - `SResidueKey` doubling here as a throwaway
//  "two scratch dwords" view purely so the generic per-dword folder can
//  read it, the same repurposing residuekey.h's own header comment already
//  documents for CRandData's history buffer. The final term folds
//  `g_pTimeSync` itself, but through a **cached local** read once right
//  after `host` (`mov edi, g_pTimeSync`, pushed/popped alongside `esi`) -
//  not a fresh global read at the point of use the way the obvious
//  translation would put it; reading it there instead cost this
//  reconstruction its first attempt (`push esi` only, no `edi`, and the
//  global reloaded at the tail).
//----------------------------------------------------------------------------
unsigned int __fastcall CNetData_FoldNetChecksumLegs()
{
    CGameDataContext* context =
        (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    unsigned int netHash = GameDataContextChecksumProviderHash(context);

    void* host = g_unk6FAB778C;
    CTimeSync* timeSync = g_pTimeSync;
    unsigned int accum = netHash;

    SResidueKey scratch;

    scratch.Set(TempestProviderListHash(host));
    CTimeSync_HashInto((CTimeSync*)&scratch, &accum);

    unsigned int fallbackOrHost = *(unsigned int*)((char*)host + 0x40);
    unsigned int birthTag = (fallbackOrHost == 0xFFFFFFFFu)
                                 ? *(unsigned int*)((char*)host + 0x1C)
                                 : fallbackOrHost;
    scratch.Set(birthTag);
    CTimeSync_HashInto((CTimeSync*)&scratch, &accum);

    unsigned int presenceTag = *(unsigned int*)((char*)host + 0x50);
    scratch.Set(presenceTag);
    CTimeSync_HashInto((CTimeSync*)&scratch, &accum);

    CTimeSync_HashInto(timeSync, &accum);

    return accum;
}

//  0x6F543E50 / 0x6F537820 / 0x6F6504F0 / 0x6F2C95B0 - Net::CTurnsSyncStore,
//  built as a local by DispatchQueuedTurnRecord below to accumulate and
//  flush one applied-delta record - see netclient.h for the class itself
//  and why its own vtable stays the real shipped address rather than a
//  from-scratch C++ one.  0x6F537820 (write the delta dword) and 0x6F2C95B0
//  (destroy) are already real reconstructions elsewhere in this repo under
//  the same addresses, reached from a different closure
//  (ServerTock_0x27_PacketHandler's own CDataStoreCache1460 local -
//  cdatastorecache.cpp): `NetMsgWriteDword_6F537820` (netmsgio_537.cpp) is
//  the exact same "WriteDword(*src); return self;" shape this dump's own
//  body shows, and `CDataStoreCache1460::~CDataStoreCache1460`'s own body
//  is exactly this destroy call - CTurnsSyncStore adds no destructible
//  members and no vtable dtor override of its own, so the compiler
//  resolves "destroy this CTurnsSyncStore" directly to the base's
//  destructor (the ctor still stamps the derived vtable; nothing here
//  ever restamps it back before destruction, matching this dump exactly).
//  Calling those two directly - rather than a second naked thunk to the
//  same two addresses under new local names - is CLAUDE.md's own
//  "grep src/ for its address" rule: a hit means call the reconstruction
//  that is already there.
CDataStoreScratch* __fastcall NetMsgWriteDword_6F537820(CDataStoreScratch* self, const unsigned int* src);
void __fastcall CDataStoreCache1460_ConstructInPlace(void* self);
void __fastcall CDataStoreCache1460_DestroyInPlace(void* self);

void __fastcall CNetData_AppliedDeltaReporterCtor(CTurnsSyncStore* self)
{
    CDataStoreCache1460_ConstructInPlace(self);
    self->m_vtable = (void*)kTurnsSyncStoreVftable;
    self->ReserveRecord();
}

//  `void*` return (the reporter's own field pointer - unused by every
//  caller, but part of the mangled name), matching netevent_dispatch.cpp's
//  own re-declaration of this same address (its own
//  CNetEventTypeDispatcher_ConstructAndBroadcast closure reaches this
//  too) - two different return types across TUs mangle to two different
//  symbols and link_check.py catches it as a real link failure either way
//  round, so this is the one both files now agree on.
void* __fastcall CNetData_AppliedDeltaReporterSetTurnField(void* self, void* turnFieldPtr)
{
    return NetMsgWriteDword_6F537820((CDataStoreScratch*)self, (const unsigned int*)turnFieldPtr);
}

void __fastcall CNetData_AppliedDeltaReporterDtor(void* self)
{
    CDataStoreCache1460_DestroyInPlace(self);
}

//  DispatchQueuedTurnRecord's own tail: build/use/destroy a CTurnsSyncStore
//  exactly once, on the "active player, delta applied" path.
static void __fastcall CNetData_ReportAppliedTurnDelta(SNetSessionInfo* self, void* /*unused_edx*/, unsigned int /*delta*/)
{
    CTurnsSyncStore reporter;
    CNetData_AppliedDeltaReporterCtor(&reporter);
    CNetData_AppliedDeltaReporterSetTurnField(&reporter, (char*)self + 0x1C64);
    reporter.Flush();
    CNetData_AppliedDeltaReporterDtor(&reporter);
}

//  The two four-byte session tags this function's own gate compares
//  against - "POOL" and "NONE" (0x4C4F4F50 / kNetSessionTagNone), read
//  the same little-endian-first-character way kNetSessionTagNone/
//  kNetSessionTagBattleNet already are in netdata.h.  "POOL" is new to
//  this closure and not yet named there.
const unsigned int kNetSessionTagPool = 0x4C4F4F50u;


//----------------------------------------------------------------------------
//  0x6F550730 - CNetData::DispatchActionByte.  See the file header above
//  and docs/targets/CNetData_DispatchActionByte.md for the full walkthrough.
//  Returns the two-byte record-count prefix read off the outer stream (0 on
//  either overflow guard tripping) - the same "return the count on success,
//  0 on failure" shape the two `movzx eax,[var_970]` / `xor eax,eax` exits
//  in the dump give.
//----------------------------------------------------------------------------
unsigned int __fastcall CNetData_DispatchActionByte(
    SNetSessionInfo* self, void* /*unused_edx*/,
    CDataStore* record, unsigned int playerIndex)
{
    CNetData* nd = (CNetData*)self;

    self->m_dispatchLatched = 0;

    unsigned short recordCount = 0;
    NetMsgReadWord_6F6516C0(record, &recordCount);

    if ((unsigned int)record->m_readPos > (unsigned int)record->m_field10)
    {
        CNetData_ParseError("Net::EVENT_TURNS");
        return 0;
    }

    unsigned int index = playerIndex;
    if (index == 0 && g_unk6FAB65F4 == 0)
        return 0;

    //  The per-record dispatch check every case shares: a vtable call
    //  through the outer stream's own slot 6 (+0x18) - not modelled as a
    //  virtual on CDataStore itself (its own declared vtable only reaches
    //  slot 2/Grow; adding a fourth slot here purely for this one call
    //  site risks disagreeing with check_vtables.py's committed layout for
    //  a class other code already relies on), so read through the raw
    //  slot instead, the same shape CLAUDE.md's own vtable-dispatch
    //  example uses.  Recomputed fresh at both call sites (here and at the
    //  bottom of the loop below) rather than cached in a variable - the
    //  dump does the same lookup twice rather than keeping the function
    //  pointer live across the loop, and caching it here cost this
    //  reconstruction a stack spill that shows up nowhere in the real body.
    typedef int (__fastcall *RecordGateFn)(CDataStore*);
    if ((*(RecordGateFn**)record)[6](record))
        return recordCount;

    //  0x6F652300 (CNetData_ReadSenderKeyAndLength) is not just a 3-byte
    //  header read - its own second call (0x6F651AB0, whose body this
    //  session did chase, one level past the original depth-2 walk) reads
    //  a 2-byte length prefix *and then copies that many raw bytes*
    //  (capped at 0x400) off the outer stream into the same destination
    //  buffer, 2 bytes past its start.  So `header` below is the *whole*
    //  sub-record: byte 0 is the sender-lookup key, bytes 1-2 are the
    //  length prefix, and bytes 3 onward are the raw sub-record payload
    //  the per-sub-record `reader` CDataStore actually wraps - not empty
    //  scratch space, as an earlier pass through this reconstruction
    //  assumed.  The wire action byte itself is the *first byte of that
    //  payload*, read through `reader`, not through `record` - `reader`
    //  is a view over bytes this call just populated, and every
    //  CNetCommandXxx builder's own Attach hook keeps reading from the
    //  same `reader` right where this leaves off.
    //
    //  This is genuinely a *two-level* loop, which an earlier pass through
    //  this reconstruction flattened into one and got wrong: the outer
    //  level (loc_6F5507E0 in the dump) reads one sub-record header off
    //  `record` at a time; the inner level (loc_6F550870) keeps dispatching
    //  action bytes out of the *same* `reader` for as long as it has more
    //  than one packed into it, only falling back to the outer level once
    //  `reader` is fully consumed.  An unrecognised action byte (the
    //  `default` case below) skips the inner level's own "more bytes left"
    //  check entirely and falls straight through to the shared "commandId"
    //  tail, exactly as the dump's `jz def_6F550921` does.
    for (;;)   // outer: one sub-record header per iteration
    {
        //  Only the 2-byte length-prefix slot is pre-zeroed (`mov word
        //  ptr [var_414+1], bx` in the dump), not the whole buffer - it is
        //  ReadWord's own default-to-0-if-EnsureReadable-fails value, not
        //  a defensive whole-array clear.  Zero-initialising all 1027
        //  bytes here would cost a `memset`-shaped loop the dump has
        //  nothing to match.
        unsigned char header[3 + 0x400];
        *(unsigned short*)&header[1] = 0;
        CNetData_ReadSenderKeyAndLength(record, header);

        if ((unsigned int)record->m_readPos > (unsigned int)record->m_field10)
        {
            CNetData_ParseError("TURNDATA");
            return recordCount;
        }

        unsigned short subRecordKey = header[0];
        unsigned short subRecordLen = *(unsigned short*)&header[1];
        CNetPlayerRecord* player = (CNetPlayerRecord*)((char*)nd + index * 0x304 + 8);
        unsigned char sender = CNetData_SenderFromKey(&nd[index].m_slots, 0, subRecordKey);

        //  EnsureReadable's own "already readable" fast path
        //  (m_field8=0, m_field0xC=-1) is what lets `reader` treat
        //  `header+3` as a plain contiguous buffer of `subRecordLen`
        //  bytes - the same trick every already-reconstructed
        //  CNetCommandXxx Attach hook relies on.
        CDataStore reader;
        reader.m_field4   = header + 3;
        reader.m_field8   = 0;
        reader.m_field0xC = -1;
        reader.m_field10  = subRecordLen;
        reader.m_readPos  = 0;

    for (;;)   // inner: every action byte packed into this one `reader`
    {
        unsigned char wireByte = 0;
        reader.ReadByte(&wireByte);

        //  The cheat-permission gate: `wireByte` also drives the 133-case
        //  dispatch below, so this is computed before the switch even
        //  though the dump computes it in the same straight-line block.
        int suppressFire = 0;
        if (index != 0)
        {
            //  jnz loc_5508FD in the dump - the whole permission block
            //  below is skipped for a non-default player index, and
            //  suppressFire keeps whatever it already was (0 here; see
            //  docs/targets/CNetData_DispatchActionByte.md for why this
            //  corner is not fully proven).
        }
        else if (wireByte > 0x0C)
        {
            suppressFire = 1;
        }
        else
        {
            void* holderEntry = (void*)IndexedArrayHolder_At(g_unk6FAB65F4, wireByte);
            suppressFire = CNetData_CheckCheatPermission(holderEntry, wireByte);
        }

        if (nd->m_pausingSlot != 0xFF && (unsigned int)(wireByte - 1) <= 29)
        {
            //  byte_6F551518 - a 30-entry table collapsing every raw
            //  action byte 1..30 into one of two outcomes: 2-15 and
            //  22-28 are hard-blocked (forced suppressFire=1) regardless
            //  of the permission check above; 1, 16-21, 29 and 30 leave
            //  suppressFire exactly as the permission check above already
            //  set it.  Read with `ida_query bytes 0x6F551518 30` - see
            //  docs/targets/CNetData_DispatchActionByte.md for the byte
            //  values and the derivation.
            static const bool kHardBlocked[30] = {
                false, true,  true,  true,  true,  true,  true,  true,
                true,  true,  true,  true,  true,  true,  true,  false,
                false, false, false, false, false, true,  true,  true,
                true,  true,  true,  true,  false, false
            };
            if (kHardBlocked[wireByte - 1])
                suppressFire = 1;
            //  The nested `[esi+1C74h]`/"byte in [0x20,0x32]" fallback the
            //  dump's own default label carries is unreachable from this
            //  branch (wireByte<=30 here, well under 0x20..0x32's own
            //  lower bound only when wireByte<0x20, which is always true
            //  in this range) - see the writeup for the one case
            //  (wireByte in [0x20,0x1E]... i.e. never, since 0x1E=30<0x20)
            //  where that matters instead: the *default* path below.
        }
        else if (self->m_cheatsEnabled != 0 && wireByte >= 0x20 && wireByte <= 0x32)
        {
            suppressFire = 1;
        }

        //  `turn`/`replayFlag` are deliberately NOT hoisted into locals here:
        //  the dump re-reads `[esi+1C68h]`/`[esi+2274h]` fresh inside every
        //  one of the 68 case bodies below (`mov edx,[esi+1C68h]` /
        //  `mov edx,[esi+2274h]`, repeated per case) rather than caching them
        //  once before the switch.  Hoisting them let the compiler keep them
        //  live in esi/edi across the whole switch instead, which cost this
        //  reconstruction most of its score the first time through - see
        //  docs/targets/CNetData_DispatchActionByte.md.
        switch (wireByte)
        {
        case 1: CNetCommandPause_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 2: CNetCommandUnpause_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 3: CNetCommandGameSpeed_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 4: CNetCommandGameSpeedIncrement_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 5: CNetCommandGameSpeedDecrement_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 7: CNetCommandSaveDone_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 16: CNetCommandUnitOrderBasic_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 17: CNetCommandUnitOrderTargetPoint_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 18: CNetCommandUnitOrderTargetImage_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 19: CNetCommandUnitOrderTargetImage2_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 20: CNetCommandUnitOrderTargetImageFogged_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 21: CNetCommandUnitOrderTargetImageFogged2_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 22: CNetCommandUnitSelectionModify_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 23: CNetCommandUnitDefineControlGroup_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 24: CNetCommandUnitSelectControlGroup_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 25: CNetCommandUnitSelectSubGroup_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 26: CNetCommandUnitRefreshSubGroup_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 27: CNetCommandUnitSelectionEvent_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 28: CNetCommandSelectableSelectionModify_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 29: CNetCommandReviveCancel_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 30: CNetCommandTrainCancel_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 32: CNetCommandCheatCooldown_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 33: CNetCommandCheatCritter_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 34: CNetCommandCheatDefeat_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 35: CNetCommandCheatFastBuild_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 36: CNetCommandCheatFastDeath_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 37: CNetCommandCheatFood_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 38: CNetCommandCheatGod_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 39: CNetCommandCheatGold_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 40: CNetCommandCheatLumber_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 41: CNetCommandCheatMana_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 42: CNetCommandCheatNoDefeat_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 43: CNetCommandCheatNoVictory_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 44: CNetCommandCheatResearch_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 45: CNetCommandCheatResources_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 46: CNetCommandCheatSetTOD_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 47: CNetCommandCheatShowMap_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 48: CNetCommandCheatTechTree_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 49: CNetCommandCheatUpgrade_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 50: CNetCommandCheatVictory_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 80: CNetCommandTeamChangeAlliance_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 81: CNetCommandTeamTradeResources_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 96: CNetCommandPlayerChatMatch_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 97: CNetCommandPlayerEndCinematic_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 98: CNetCommandResumeTriggerExec_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 99: CNetCommandTriggerSyncReady_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 100: CNetCommandTrackableHit_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 101: CNetCommandTrackableTrack_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 102: CNetCommandShowSkill_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 103: CNetCommandShowSubmenu_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 104: CNetCommandAllyPing_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 105: CNetCommandDialogButtonClick_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 106: CNetCommandDialogClick_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 107: CNetCommandSyncStoreInteger_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 108: CNetCommandSyncStoreReal_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 109: CNetCommandSyncStoreBoolean_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 110: CNetCommandSyncStoreUnit_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 112: CNetCommandSyncClearInteger_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 113: CNetCommandSyncClearReal_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 114: CNetCommandSyncClearBoolean_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 115: CNetCommandSyncClearUnit_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 117: CNetCommandArrowKey_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 132: CNetCommandReplayViewSpeedIncrement_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;
        case 133: CNetCommandReplayViewSpeedDecrement_BuildAndFire(player, (int)playerIndex, &reader, sender, "", suppressFire, self->m_replayFlag, self->m_currentTurn, 0); break;

        case 6:
            //  The one case with no CNetCommandXxx builder at all: when
            //  the permission gate above did not suppress it, latch this
            //  action's player index/sender into the session (read by
            //  nothing else in this closure) instead of firing anything.
            //  Otherwise, fall to the same "unknown action" diagnostic the
            //  default case below uses.
            if (suppressFire == 0)
            {
                CNetData_RememberLastAction(&reader, (char*)self + 0x1B48);
                self->m_dispatchLatchedPlayerIndex = index;
                self->m_dispatchLatchedSender = sender;
                self->m_dispatchLatched = 1;
            }
            else
            {
                unsigned char diag[sizeof(CNetCommandSave)];
                CNetCommandSave* save = new (diag) CNetCommandSave(6);
                CNetData_RememberLastAction(&reader, save);
            }
            break;

        case 129:
        {
            CNetCommandReplayDataUncompressed cmd;
            cmd.m_sender = sender;
            CNetCommandReplayDataUncompressed_Attach(&reader, &cmd);
            if (index == 1 && sender == 0)
                CNetData_SubmitReplayPayload(self, 0, &cmd, cmd.m_dataSize, 0);
            break;
        }
        case 130:
        {
            CNetCommandReplayDataCompressed cmd;
            cmd.m_sender = sender;
            CNetCommandReplayDataCompressed_Attach(&reader, &cmd);
            if (index == 1 && sender == 0)
                CNetData_SubmitReplayPayload(self, 0, &cmd, cmd.m_dataSize, (int)index);
            break;
        }
        case 131:
        {
            //  0x6F554570 - already reconstructed as NetMsgIoNoFields_6F554570
            //  (netmsgio_554.cpp): the message struct has no serialised
            //  fields, so this "Attach" is the fluent `return this` no-op.
            CNetCommandReplayDataDone cmd;
            cmd.m_sender = sender;
            //  reader->Attach(&cmd) here is a no-op (already reconstructed
            //  at this address as NetMsgIoNoFields_6F554570,
            //  netmsgio_554.cpp - not called by symbol here because that
            //  declaration's own CDataStoreScratch* parameter type would
            //  give this call site a different, non-matching mangled name).
            //
            //  The dump's own tail here is NOT a CNetData_FireCommand call -
            //  it calls 0x6F537B10, already reconstructed as
            //  SetField0xAD0_6F537B10 (trivialaccessors_05.cpp: "store 1
            //  into the dword at +0xAD0"), on `self` (esi), not on `player`
            //  or `&cmd` at all.
            if (index == 1 && sender == 0)
                SetField0xAD0_6F537B10(self);
            break;
        }

        default:
            //  An unrecognised action byte skips straight past the
            //  "does `reader` have more?" check below - `jz def_6F550921`
            //  in the dump reaches this same tail from the dispatch guard
            //  directly, not through loc_6F551475's own readPos compare.
            goto unknownOrExhausted;
        }

        //  loc_6F551475 - after a matched case fires, loop back
        //  (loc_6F550870, i.e. `continue` the inner loop) for another
        //  action byte out of the *same* `reader` if it isn't fully
        //  consumed yet; only once it is does this fall through to the
        //  shared "commandId" tail both paths converge on.
        if (reader.m_readPos != reader.m_field10)
            continue;

    unknownOrExhausted:
        CNetData_ParseError("commandId");
        //  ~CDataStore's own body (m_field0xC != -1 -> recycle the
        //  backing buffer) never actually triggers here: `reader` is
        //  always constructed with m_field0xC == -1 above, so this reader
        //  never owns pool-recycled storage - the same accepted gap
        //  netcommand.h's own header comment already documents for every
        //  sibling CNetCommandXxx builder's local command object.
        if ((*(RecordGateFn**)record)[6](record))
            return recordCount;
        break;   // out of the inner loop; loc_6F5507E0 reads the next
                 // sub-record header off `record` in the outer loop
    }
    }
}
//----------------------------------------------------------------------------
//  0x6F5516E0 - CNetData::DispatchQueuedTurnRecord.  Formerly thunked as
//  CNetData_ParseTurnCommandStream in pausecallchainthunks.cpp under a name
//  Net wave 13 gave it *without* reading its own body - the live call
//  stack only proved it sits between the CNetEvent dispatcher and
//  DispatchActionByte, not what it does once there.  Reading the dump
//  (asm/sub_6F5516E0_0x6F5516E0_calltree_asm.md) shows it calls
//  DispatchActionByte exactly *once* per call, not in a loop over an
//  embedded stream the way that old name implies - it decodes one queued
//  turn record, updates the per-player turn-rate/pending-resync
//  bookkeeping, and conditionally fires a tick-notification event.
//  Renamed here to match what the disassembly actually shows; see
//  docs/targets/CNetData_DispatchActionByte.md for the full note on the
//  discrepancy with the name Net wave 13 gave it going in.  `retn 0Ch`
//  confirmed both by the dump and by the thunk it replaces.
//----------------------------------------------------------------------------
void __fastcall CNetData_DispatchQueuedTurnRecord(
    SNetSessionInfo* self, void* /*unused_edx*/,
    CDataStore* record, unsigned int playerIndex, unsigned int latchedDelta)
{
    if (playerIndex == 0)
        self->m_currentTurn += 1;

    unsigned int consumed = CNetData_DispatchActionByte(self, 0, record, playerIndex);
    unsigned int delta = (consumed != 0) ? latchedDelta : 0;

    char* playerRecord = (char*)self + playerIndex * 0x304;
    *(unsigned int*)(playerRecord + 0x284) += consumed;
    self->m_turnRateAccumulator -= consumed;

    if (delta != 0)
    {
        //  x*7/8 + consumed/8 - the same moving-average shape the dump's
        //  own `lea edi,[edx*8]; sub edi,edx; add edi,eax; shr edi,3`
        //  spells.
        unsigned int x = self->m_turnRateAccumulator;
        self->m_turnRateAccumulator = (x * 8 - x + consumed) >> 3;

        int* pendingResync = (int*)(playerRecord + 0x2E0);   // CNetPlayerRecord::m_suspendAlloc
        if (*pendingResync != 0)
        {
            unsigned int clamped = ((unsigned int)*pendingResync > consumed) ? consumed : (unsigned int)*pendingResync;
            *pendingResync -= clamped;
            if (*pendingResync == 0)
                CNetData_NotifyResyncComplete(self, 0, playerIndex, 0);
        }
    }

    bool notifyGate = (playerIndex == 0) && (delta != 0);
    bool flagBE4Set = (self->m_flagBE4 == 1);
    bool activeFlagsClear = (self->m_flag2270 == 0 && self->m_flag226C == 0 && self->m_activeIndex == 1);
    unsigned int sessionTag = self->m_sessionTag;
    bool tagAllows = (sessionTag != kNetSessionTagPool && sessionTag != kNetSessionTagNone &&
                      self->m_activeIndex == 0);

    if (notifyGate && (flagBE4Set || activeFlagsClear || tagAllows))
        CNetData_FireTickNotification(self);

    if (playerIndex == (unsigned int)self->m_activeIndex && delta != 0)
        CNetData_ReportAppliedTurnDelta(self, 0, delta);
}
