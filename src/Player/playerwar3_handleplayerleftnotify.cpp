//============================================================================
//  0x6F41F340 - CPlayerWar3HandlePlayerLeftNotify, case 7 of
//  CPlayerWar3::Method_0x0C's own 8-way switch (playerwar3_method0x0c.cpp).
//  Publishes (or suppresses) the "a player has left the game" chat
//  announcement and, on one path, a further world/GameUI cleanup tail.
//
//  This is genuinely the largest closure this target's own dump reaches
//  short of CSelectionWar3::Load's (see docs/targets/CPlayerWar3.md).  Its
//  real depth-1 closure is written out in full below; two of its own
//  callees stay naked thunks, each for a reason this repo has an
//  established precedent for rather than a shortcut:
//
//    * sub_6F41B4C0 (`retn 0`, called unconditionally from this function's
//      own depth-1 helper below) recomputes the player's own +0x2E0
//      ally-visibility bitmask by walking all sixteen world-table slots
//      through sub_6F41B420 - itself a lazy `SCheckedAllySlot` accessor
//      (agenttypedslots.h, 0x6F40CC60) - and, on one path, tail-calls
//      CWorldFrameWar3::LatchLocalPlayer (already reconstructed,
//      GameUI/worldframelocalplayer.cpp) after touching two more
//      per-world-slot fields (+0x3C/+0x34) this repo has no struct for.
//      Fully enumerable, but genuinely large (roughly 90 further
//      instructions across six more callees, half of them new), and this
//      session's own budget went to the shallower closure below instead;
//      left for a follow-up pass rather than guessed at under time
//      pressure. `retn 0` read directly off the dump's own bare `retn`.
//    * sub_6F2F9AC0, reached only via a genuine tail-jmp (not a `call`) on
//      one path of this function's own body, takes `ecx` = the `SGameUI*`
//      GetGameUI(1,0) returns (gameui.h's own opaque forward declaration -
//      never given a concrete layout anywhere in this repo) and walks
//      fields at +0x24C/+0x3BC/+0x3D0 of *that* object, none of which
//      match `CGameUI`'s own already-modelled layout at those offsets
//      (gameui.h) - a different, unmodelled type sharing the name only in
//      spirit. `retn 0` read directly off the dump's own bare `retn`
//      (the tail-jmp's own `add esp,4` is this function's own frame
//      teardown, not part of the callee's stack contract).
//
//  See tools/funcmap.py's own BEHAVIOUR entries for the exact scores.
//============================================================================
#include "playerwar3.h"
#include "game.h"

//----------------------------------------------------------------------------
//  0x6F473170 - already reconstructed (Misc/misc_handle_lookups.cpp).
//  Redeclared identically, matching every other user of this exact shape
//  in this module (playerwar3_method0x0c.cpp).
//----------------------------------------------------------------------------
struct HandleRefFieldOwner_6F473170
{
    char pad[8];
    unsigned int m_handle;   // +8
    int          m_typeTag;  // +0xC

    int QueryField78();
    void SetField78(int value);
};

//----------------------------------------------------------------------------
//  0x6F3A1650 - IndexedArrayHolder_6F3A1650::At, already reconstructed and
//  EXACT (Misc/misc_field_getters.cpp).  Declared locally the way every
//  other caller in this repo does (see GameUI/gameuiplayername.cpp's own
//  comment on why: a second naked redirect at the same address would be
//  two symbols for one function and would cost the real body its score).
//
//  This same singleton (dword_6FAB65F4) is *also* `CJassGameStateMasks`
//  (Agent/agenteventbinding.h's own header comment: "CJassGameStateMasks
//  and IndexedArrayHolder_6F3A1650 are two views of the one object") -
//  its own +0x30 field (`m_pGameState`, a `SJassGameStateBlock*`) is what
//  this function's own `record`/`m_flags38` read below actually is; using
//  that already-established type instead of guessing a new one.
//----------------------------------------------------------------------------
#include "agenteventbinding.h"

struct IndexedArrayHolder_6F3A1650
{
    char           m_reserved00[0x28];
    unsigned short m_actingSlot;   // +0x28
    int At(unsigned int index);
};

//  dword_6FAB65F4 - the player-slot table singleton, already named
//  elsewhere (Widget/playercolor.h) - redeclared identically here.
extern void* g_unk6FAB65F4;

//  0x6F53F160 - IsGameModeOne, already reconstructed and EXACT
//  (Game/gamemode.cpp).
int __fastcall IsGameModeOne();

//  0x6F300710 - GetGameUI, already reconstructed (Player/
//  playerscore_tallyapply.cpp).  SGameUI stays the same forward-declared
//  opaque type it already is everywhere else in this repo - see this
//  file's own header comment for why sub_6F2F9AC0's own reach into it
//  cannot be modelled concretely.
struct SGameUI;
SGameUI* __fastcall GetGameUI(int create, int reset);

//  0x6F54C7E0 - EndSessionGame, already reconstructed (Net/
//  netsessionend.cpp).
void __fastcall EndSessionGame(unsigned int eventId, int eventArg, int index);

//----------------------------------------------------------------------------
//  0x6F41B4C0 - see this file's own header comment above.
//----------------------------------------------------------------------------
__declspec(naked) void __fastcall CPlayerWar3FinishLeaveNotify(CPlayerWar3*)
{
    __asm
    {
        mov     eax, 06F41B4C0h
        jmp     eax
    }
}

//----------------------------------------------------------------------------
//  0x6F2F9AC0 - reached only via tail-jmp on the SGameUI-cleanup path.  See
//  this file's own header comment.  `retn 0` - a plain `void(SGameUI*)`.
//----------------------------------------------------------------------------
__declspec(naked) void __fastcall PlayerLeftGameUICleanupThunk(SGameUI*)
{
    __asm
    {
        mov     eax, 06F2F9AC0h
        jmp     eax
    }
}

//----------------------------------------------------------------------------
//  0x6F426090 - a CSelectionWar3 method ("clear sync selection for
//  player") reaching CUnitSet's own never-modelled vtable/list - see
//  playerwar3_ctorsubobjects.cpp's own header comment for the same
//  boundary.  `retn 8` matches its own two *stack* arguments (a flag, then
//  a literal 0 - both on the stack, not the `ecx`/`edx` pair a
//  `__fastcall` declaration would claim). `this` is a
//  CPlayerWar3::m_selectionWar3 (+0x34, playerwar3.h). A member of a bare
//  overlay struct for the same VC8-cannot-spell-`__thiscall`-on-a-free-
//  function reason SPlayerLeaveHelper below is.
//----------------------------------------------------------------------------
struct SSelectionWar3SyncHelper
{
    void ClearSyncSelection(unsigned int flag, int zero);
};

__declspec(naked) void SSelectionWar3SyncHelper::ClearSyncSelection(unsigned int, int)
{
    __asm
    {
        mov     eax, 06F426090h
        jmp     eax
    }
}

//----------------------------------------------------------------------------
//  0x6F41F100 - publish whether the ability-array element 1's own real
//  slot value (`slotValue` - the same value the root read out of its own
//  +0x40 element and passes straight through) is non-zero into +0xF0,
//  clear the player's own sync selection through the base class's own
//  Deactivate-adjacent path when it is, and always finish through
//  CPlayerWar3FinishLeaveNotify (a real tail call in the shipped stream -
//  its own result is this function's own return value).
//
//  `slotValue` arrives as a genuine stack argument (`retn 4`) - `self` is
//  the only register-passed argument (`ecx`), a real `this`, not a
//  `__fastcall` pair (which would also claim `edx`).  VC8 will not accept
//  `__thiscall` on a free function, so this is spelled as a member of a
//  bare overlay struct the same way every other single-`ecx`-plus-stack-
//  args leaf in this repo is (see IntegerListenerBindHelper in
//  integerlistenerbindthunk.cpp for the precedent).
//----------------------------------------------------------------------------
struct SPlayerLeaveHelper
{
    void PublishAndClearSyncSelection(int slotValue);
};

void SPlayerLeaveHelper::PublishAndClearSyncSelection(int slotValue)
{
    CPlayerWar3* self = (CPlayerWar3*)this;

    ((HandleRefFieldOwner_6F473170*)((char*)self + 0xF0))->SetField78(slotValue != 0);

    if (slotValue != 0)
    {
        SSelectionWar3SyncHelper* selectionWar3 =
            (SSelectionWar3SyncHelper*)self->m_selectionWar3;
        selectionWar3->ClearSyncSelection(self->m_flag30, 0);
    }

    CPlayerWar3FinishLeaveNotify(self);
}

//----------------------------------------------------------------------------
//  0x6F41F340 - see this file's own header comment.
//
//  `currentRecord` is the world table's own entry at the acting slot's
//  index (IndexedArrayHolder_6F3A1650::At(actingSlot)) - only ever used to
//  confirm `self` really is that acting record before publishing anything,
//  which is why it is read once, stashed, and compared against `self`
//  rather than dereferenced.
//
//  `ability1Value` is the ability-array's own element 0's QueryField78
//  (m_abilityEntries+0x40, playerwar3.h - the array's own first element,
//  stride 0x10 from +0x40; confirmed against the dump's own
//  `lea ecx,[edi+64]`, distinct from Method_0x0C's own case 0, which reads
//  *element 1* at +0x50) - used raw here as a 0/1/2 selector.
//  `earlyRunFlag` mirrors
//  the shipped stream's own `ebp`: it is only ever `ability1Value` itself
//  (1, since it is only assigned inside the `ability1Value == 1` arm) or 0,
//  so it is written that way rather than as a separate boolean to keep the
//  exact value the case-1 arm below re-tests.
//----------------------------------------------------------------------------
void __fastcall CPlayerWar3HandlePlayerLeftNotify(CPlayerWar3* self)
{
    IndexedArrayHolder_6F3A1650* world =
        (IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4;
    int currentRecord = world->At(world->m_actingSlot);

    int ability1Value =
        ((HandleRefFieldOwner_6F473170*)((char*)self + 0x40))->QueryField78();

    int earlyRunFlag = 0;

    if (ability1Value == 1)
    {
        SJassGameStateBlock* gameState = ((CJassGameStateMasks*)world)->m_pGameState;

        if (gameState != 0 && (gameState->m_flags38 & 0x20))
        {
            earlyRunFlag = ability1Value;
            ((SPlayerLeaveHelper*)self)->PublishAndClearSyncSelection(ability1Value);
            ((HandleRefFieldOwner_6F473170*)((char*)self + 0xE0))->SetField78(earlyRunFlag);
        }
    }

    if (IsGameModeOne())
        return;
    if ((CPlayerWar3*)currentRecord != self)
        return;

    switch (ability1Value)
    {
    case 0:
        EndSessionGame(9, 1, 0);
        break;
    case 1:
        if (earlyRunFlag)
        {
            SGameUI* gameUI = GetGameUI(1, 0);
            if (gameUI != 0)
                return PlayerLeftGameUICleanupThunk(gameUI);
        }
        else
        {
            EndSessionGame(8, 1, 0);
        }
        break;
    case 2:
        EndSessionGame(0xA, 1, 0);
        break;
    default:
        break;
    }
}
