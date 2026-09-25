//============================================================================
//  0x6F40A650 - SPlayerTableSubmitParts::RefreshUnitFog, per the
//  desync-mechanism worklist's "closure650" batch
//  (docs/notes/checksum-provider-registry.md).  Flagged by an earlier
//  batch (checksumE) as "dumped but not yet reconstructed"; its own dump
//  (depth 1) is asm/sub_6F40A650_0x6F40A650_calltree_asm.md.
//
//  `unit` has flag 0x100 set without flag 0x20 - i.e. some "suppress fog
//  refresh" state - bails immediately.  Otherwise:
//
//    1. Add `unit` to the local world frame's own selected-units array
//       unless it is already reachable through CUnit's own special-
//       selection query (CWorldFrameWar3::AddSelectedUnitIfNotAlready,
//       0x6F39A1E0, GameUI/worldframeaddselectedunit.cpp).
//    2. **The second stack argument is a sentinel, not dead.**  Its low
//       16 bits are compared against 0xFFFF *before* anything overwrites
//       the slot - `presetMaskOrAll` in the signature below - and only a
//       literal 0xFFFF asks this function to compute a fresh mask; any
//       other value is used as the fog mask outright and every step below
//       is skipped.  (An earlier reading of this dump treated the second
//       argument as always-dead, since the compiler reuses its very own
//       stack slot as the `fogMask` local computed below - which does
//       make it dead *after* this first read, but not before it.)
//    3. To compute fresh: resolve `unit`'s owning player's record out of
//       the player-slot table (the same g_unk6FAB65F4/
//       AcquireGameSingleton(6,0) lazy acquire
//       GameUI/worldframelocalplayer.cpp and
//       Jass/jassfogstateplayermask.cpp already use), and its relation
//       agent (SJassPlayerRelations::EnsureRelationAgent,
//       Agent/jassrelationagent.cpp) - fetched *twice*, once per field
//       read, exactly like CJassGameStateMasks::DetectedMask
//       (Agent/jassgamestatemasks.cpp) already does for the same reason.
//       OR three per-unit query results together into one 16-bit fog
//       mask: the relation agent's own `+0x88` and `+0xC8` handle-ref
//       fields, and CUnit::QuerySpecialSelectionMask() - then, only if
//       CUnit::QueryVisibleImpl(1) says the unit is *not* already visible,
//       fold in CUnit::QueryLegacyVisionWord() as well.
//    4. Either way, refresh two more CUnit-owned caches (RefreshField10C,
//       RefreshRelationMaskFields) whose own results this function does
//       not read.
//    5. Forward `unit` and the assembled mask to ApplyUnitFogRadius
//       (still a thunk - see playertablefog.h).
//
//  The `+0x88` read goes through HandleRefFieldOwner_6F473170::QueryField78
//  as an ordinary member call (`lea ecx,[eax+88h]` / `call sub_6F473170`);
//  the `+0xC8` read does not - the shipped code passes the *agent itself*
//  into the existing `sub_6F3DF1C0` adjustor (`add ecx,0C8h` / `jmp
//  QueryField78`, Misc/misc_handle_lookups.cpp) rather than materialising
//  the same `lea`+`call` a second time, so this is spelled as a direct call
//  to that adjustor (QueryField78At0xC8) instead of a second
//  `m_reservedXX.QueryField78()` member access, to keep the instruction
//  stream identical.
//============================================================================
#include "gameui.h"
#include "jassrelationagent.h"
#include "playertablefog.h"
#include "unit.h"

//  dword_6FAB65F4 - the player-slot table singleton.  Declared, never
//  defined: it is a funcmap.DATA global (tools/link_check.py, cause 5).
extern void* g_unk6FAB65F4;

//  0x6F0074F0 - GameUI/acquiregamesingleton.cpp.
void* __fastcall AcquireGameSingleton(int index, int unused);

//  Re-declared to match Misc/misc_field_getters.cpp's own definition
//  exactly, the way every caller of IndexedArrayHolder_6F3A1650::At in
//  this repo declares its own narrow view.
struct IndexedArrayHolder_6F3A1650
{
    int At(unsigned int index);
};

//  0x6F3DF1C0's own adjustor target, `__fastcall`'d for a real call site
//  now - see Misc/misc_handle_lookups.cpp.
int __fastcall QueryField78At0xC8(void*);

void SPlayerTableSubmitParts::RefreshUnitFog(CUnit* unit, unsigned int presetMaskOrAll)
{
    if (unit == 0)
        return;

    unsigned int flags = unit->m_flags5C;
    if ((flags & 0x100) != 0 && (flags & 0x20) == 0)
        return;

    CGameUI* gameUi = (CGameUI*)GetGameUI(1, 0);
    gameUi->m_pWorldFrameWar3->AddSelectedUnitIfNotAlready(unit);

    unsigned int fogMask;
    if ((short)presetMaskOrAll == -1)
    {
        void* table = g_unk6FAB65F4;
        if (table == 0)
        {
            table = AcquireGameSingleton(6, 0);
            g_unk6FAB65F4 = table;
        }

        SJassPlayerRelations* record = (SJassPlayerRelations*)
            ((IndexedArrayHolder_6F3A1650*)table)->At(unit->GetOwningPlayerIndex());

        SJassRelationAgent* agent = record->EnsureRelationAgent();
        fogMask = (unsigned short)agent->m_reserved88.QueryField78();

        SJassRelationAgent* agent2 = record->EnsureRelationAgent();
        fogMask |= QueryField78At0xC8(agent2);

        fogMask |= unit->QuerySpecialSelectionMask();

        if (!unit->QueryVisibleImpl(1))
            fogMask |= (unsigned short)unit->QueryLegacyVisionWord();
    }
    else
    {
        fogMask = presetMaskOrAll;
    }

    unit->RefreshField10C();
    unit->RefreshRelationMaskFields();

    ApplyUnitFogRadius(unit, fogMask, 0, 0);
}

//----------------------------------------------------------------------------
//  0x6F409E00 - still a thunk.  See the declaration comment in
//  playertablefog.h.  `retn 10h` read directly off this dump's own tail
//  (asm/sub_6F40A650_0x6F40A650_calltree_asm.md, offset 0x6F40A646) -
//  four stack dwords, matching the four values RefreshUnitFog pushes.
//----------------------------------------------------------------------------
__declspec(naked) void __thiscall
SPlayerTableSubmitParts::ApplyUnitFogRadius(CUnit*, unsigned int, int, int)
{
    __asm { mov eax, 06F409E00h }
    __asm { jmp eax }
}
