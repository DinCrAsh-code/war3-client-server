//============================================================================
//  The four CUnit leaves SPlayerTableSubmitParts::RefreshUnitFog
//  (0x6F40A650, Widget/playertableunitfogrefresh.cpp) reaches straight off
//  its own `CUnit*` argument - see unit.h's own doc comments at each
//  declaration for the fog-refresh context.  All four were naked redirects
//  (formerly Unit/unit_fogrefresh_thunks.cpp); real bodies now, following
//  up on the checksum-provider-registry's own "0x6F40A650 closure" note.
//
//  checksum-round2-G (2026-09-13): the operator's bulk agent_worktrees
//  re-dump now covers sub_6F15BA20 and sub_6F165AB0 too (both were
//  genuinely `NEED_DUMP` when this file's own header note above was
//  written - no dump existed anywhere in this repo's `asm/` at the time).
//  Both are real bodies below now.  sub_6F406AB0 is also real now (see
//  its own note further down).  sub_6F29F040 alone stays thunked: its own
//  87-instruction body needs a still-`TODO` callee (sub_6F022060, its own
//  own further callee sub_6F4773A0-shaped double GetValue/operator-
//  chain) plus three more not-yet-named globals
//  (unk_6FAB44B8/dword_6FAAE568/dword_6FAAE588) - confirmed shape (a
//  config-driven CFloat comparison pair reading a "DyingRevealRadius"
//  Misc config value, per this file's own original note) but genuinely
//  deeper than this batch's checksum-family scope justifies chasing
//  further in one sitting.
//============================================================================
#include "game.h"                // LookupHandle, CHandleObject
#include "itemhandleresolve.h"   // SOptionalHandleRef
#include "jassrelationagent.h"   // SJassPlayerRelations, SJassRelationAgent
#include "unit.h"

//  dword_6FAB65F4 - the player-slot table singleton.  Declared, never
//  defined, the same funcmap.DATA global playertableunitfogrefresh.cpp
//  already declares.
extern void* g_unk6FAB65F4;

//  0x6F0074F0 - GameUI/acquiregamesingleton.cpp.
void* __fastcall AcquireGameSingleton(int index, int unused);

//  0x6F6EF330 - Math/cfloatround.cpp.  Redeclared to match that
//  definition's own free-function `__fastcall` signature exactly, same as
//  cunit_agent3_scaledroundedfloat.cpp's own redeclaration.
CFloat __fastcall RoundCFloatHalfUp(const CFloat& value);

//  Re-declared to match Misc/misc_field_getters.cpp's own definition
//  exactly, the same narrow view every caller of At() in this repo uses.
struct IndexedArrayHolder_6F3A1650
{
    int At(unsigned int index);
};

//  0x6F3DF1C0's own adjustor target - Misc/misc_handle_lookups.cpp.
int __fastcall QueryField78At0xC8(void*);

//  Re-declared to match unit_findability.cpp's own SUnitAbilityHost
//  *exactly* - same struct name, same member signature - so MSVC mangles
//  the call onto that file's real FindAbility body rather than inventing a
//  second symbol (member-function mangling folds the class name in, so
//  `SUnitAbilityHost::FindAbility` would NOT be the same symbol as
//  `SUnitAbilityHost::FindAbility` even with an identical signature - this
//  cost both CUnit::QuerySpecialSelectionMask and
//  CUnit::QueryLegacyVisionWord their own FindAbility line before the
//  struct name was fixed to match).  The same pattern
//  unit_abilitygrantflag.cpp's own SUnitAbilityHost re-declaration already
//  uses.
struct SUnitAbilityHost
{
    void* FindAbility(int rawcode, int a, int b, int c, int d);
};

//  0x6F15BA20 - `retn`, no arguments: a bare literal-constant return, `mov
//  eax, 41636172h`.  Already named `GetConstant_0x6F15BA20` in funcmap.py
//  by an earlier session (a different target's own closure reached it
//  first) - real body now, defined in Agent/cunit_agent5_agentqueries.cpp
//  (its own original home), declared `extern` here rather than given a
//  second definition, the same one-shipped-address-one-C++-name rule
//  every other redirect in this repo follows (and the same reason it is
//  its own TU: a same-file call would let this build's /Ob2 inline the
//  one-instruction body away, which the shipped code - a real, separate
//  `call sub_6F15BA20` - never does). Takes no arguments: the four
//  literal pushes at this function's one call site here (1, 0, 1, 1) are
//  FindAbility's own `a`/`b`/`c`/`d` stack slots, pushed *before* this
//  call so that its return value (the rawcode) can be pushed last, right
//  before FindAbility itself - the ordinary right-to-left argument
//  evaluation order for
//  `host->FindAbility(GetConstant_0x6F15BA20(), 1, 0, 1, 1)`.
extern unsigned int __fastcall GetConstant_0x6F15BA20();

//  0x6F165AB0 - `__thiscall(ability) -> int`, post-processes the ability
//  FindAbility(rawcode-from-above) returns.  A fixed 10-entry array of
//  {handle,typeTag} pairs starting at `ability+0x84` (stride 0xC - the
//  third dword of each entry is never read), the exact
//  `SOptionalHandleRef`/`(handle & typeTag) != -1` sentinel
//  itemhandleresolve.h's own header already documents, resolved through
//  the same `QueryHandleField0x54` this file's own
//  `QuerySpecialSelectionMask` already calls; each resolved object is
//  handed straight to `QuerySpecialSelectionMask` itself (0x6F27A460 -
//  the shipped `call sub_6F27A460` on the object `QueryHandleField0x54`
//  returned, not a *new* function) and every result is OR'd together.
struct SAbilityMaskArrayEntry
{
    unsigned int m_handle;    // +0x00
    int          m_typeTag;   // +0x04
    char         m_reserved08[0x0C - 0x08];
};

extern "C" int __fastcall Unit_QuerySpecialSelectionMaskAbilityBits(void* ability)
{
    int mask = 0;
    SAbilityMaskArrayEntry* entry =
        (SAbilityMaskArrayEntry*)((char*)ability + 0x84);

    for (int i = 0; i < 0xA; ++i, ++entry)
    {
        if ((entry->m_handle & (unsigned int)entry->m_typeTag) != 0xFFFFFFFFu)
        {
            void* resolved =
                QueryHandleField0x54((SOptionalHandleRef*)entry);
            if (resolved)
                mask |= ((CUnit*)resolved)->QuerySpecialSelectionMask();
        }
    }

    return mask;
}

//  0x6F29F040 - `retn 4`: `this` in ecx plus one stack pointer argument,
//  not a second register - a free `__fastcall` function would put that
//  argument in edx instead of on the stack (the same class of bug
//  gamewar3save.cpp/gamewar3load.cpp's own GameUI-field thunks hit
//  first), so it is a struct member instead.  Still thunked - see this
//  file's own header note on why.
struct SUnitField10CQueryReceiver
{
    unsigned short Query(void* stackArg);
};
__declspec(naked) unsigned short SUnitField10CQueryReceiver::Query(void*)
{
    __asm mov eax, 06F29F040h
    __asm jmp eax
}

//  0x6F406AB0 - `this` is the player table's own +0x34 sub-object, not a
//  CUnit; a second, unrelated struct member for the same reason as
//  0x6F29F040 above.  Real body now (checksum-round2-G): `*this+0x70` is
//  a `CFloat` (the sub-object's own maximum reveal radius), scaled by the
//  caller's by-value `CFloat` argument (`operator*`, 0x6F6EEE20) and
//  clamped against zero (`g_CFloatZero`, CFloat.h) before being rounded
//  and truncated to a 16-bit word (`RoundCFloatHalfUp` +
//  `CFloatToInt`) - a plain "round the scaled radius down to zero if it
//  would go negative" helper, the same round-half-up-then-truncate shape
//  cunit_agent3_scaledroundedfloat.cpp's own `ComputeScaledRoundedFloat`
//  already establishes for a sibling scale computation.  The caller's own
//  `CFloat` argument arrives as a raw `void*` (see
//  `CUnit::RefreshField10C`'s own comment above) - reinterpreted here by
//  address rather than by changing that established signature.
struct SPlayerTableField34QueryReceiver
{
    unsigned short Finalize(void* queryResult);
    char  m_reserved00[0x70];
    CFloat m_maxRadius;   // +0x70
};

unsigned short SPlayerTableField34QueryReceiver::Finalize(void* queryResult)
{
    CFloat product = m_maxRadius * *(const CFloat*)&queryResult;

    //  `!(product <= g_CFloatZero)` - the negated spelling
    //  docs/msvc-vc8-idioms.md documents as the one that yields
    //  `test ah,1` / `jne`, matching the shipped `fld`/`fld`/`fcompp`/
    //  `fnstsw ax`/`test ah,1`/`jnz` exactly (`product` loaded first,
    //  `g_CFloatZero` second - the same load order the source above
    //  reflects).
    if (!(*(const float*)&product.m_bits <=
          *(const float*)&g_CFloatZero.m_bits))
    {
        CFloat rounded = RoundCFloatHalfUp(product);
        return (unsigned short)CFloatToInt(rounded);
    }

    return 0;
}

//----------------------------------------------------------------------------
//  0x6F27A460.  See unit.h's own doc comment.  `m_ref13C`
//  (unit.h - a {handle,typeTag} pair, slot 13's second) resolves through
//  QueryHandleField0x54 exactly like SUnitAbilityHost::FindAbility's own
//  fast-path handle reads (Unit/unit_findability.cpp) - `result->m_field24`
//  is folded in first, unconditionally.  Then, only if CAgentWar3::m_flags
//  (inherited, +0x20) has bit 0x400000 set, a second contribution comes
//  from the ability FindAbility('Bprv'-tagged rawcode
//  GetConstant_0x6F15BA20() computes) resolves to, through
//  Unit_QuerySpecialSelectionMaskAbilityBits - OR'd into the same running
//  value.
//----------------------------------------------------------------------------
int CUnit::QuerySpecialSelectionMask()
{
    int mask;

    if ((m_ref13C.m_b & (int)m_ref13C.m_t) != -1)
    {
        void* resolved = QueryHandleField0x54((SOptionalHandleRef*)&m_ref13C);
        mask = resolved ? *(int*)((char*)resolved + 0x24) : 0;
    }
    else
    {
        mask = 0;
    }

    if (m_flags & 0x400000)
    {
        void* ability = ((SUnitAbilityHost*)this)->FindAbility(
            GetConstant_0x6F15BA20(), 1, 0, 1, 1);
        if (ability)
            mask |= Unit_QuerySpecialSelectionMaskAbilityBits(ability);
    }

    return mask;
}

//----------------------------------------------------------------------------
//  0x6F284E80.  See unit.h's own doc comment.  Bit 0x400 of CUnit's own
//  m_int60 gates the whole body - clear, and this returns 0 outright.
//  Otherwise: find the fixed 'Bprv'-rawcode ability the same way
//  unit_abilitygrantflag.cpp's own GrantFlag_6F29EE30 does
//  (FindAbility(0x42707276, 0, 0, 1, 1)), resolve its {handle,typeTag}
//  pair at +0xC/+0x10 through LookupHandle, and - only if the resolved
//  object's own kind tag is the 'lga+' (0x2B61676C) constant
//  jasssetitemposition_native.cpp's chain already checks and its
//  SItemHandleObject::m_pendingFlag is clear - read the 16-bit word at the
//  *ability's own* +0xCC (not the resolved object's).
//----------------------------------------------------------------------------
struct SAbilityWithVisionWord
{
    char         m_reserved00[0x0C];
    SOptionalHandleRef m_handleRef;   // +0x0C/+0x10
    char         m_reservedC[0xCC - 0x14];
    unsigned short m_visionWord;      // +0xCC
};

unsigned short CUnit::QueryLegacyVisionWord()
{
    if ((m_int60 & 0x400) == 0)
        return 0;

    void* found = ((SUnitAbilityHost*)this)->FindAbility(0x42707276, 0, 0, 1, 1);
    if (found == 0)
        return 0;

    SAbilityWithVisionWord* ability = (SAbilityWithVisionWord*)found;
    CHandleObject* obj = LookupHandle(ability->m_handleRef.m_handle,
                                       ability->m_handleRef.m_typeTag);
    if (obj == 0)
        return 0;

    if (obj->m_kindTag != 0x2B61676C)
        return 0;

    if (*(int*)((char*)obj + 0x20) != 0)   // SItemHandleObject::m_pendingFlag
        return 0;

    return ability->m_visionWord;
}

//----------------------------------------------------------------------------
//  0x6F29F150.  See unit.h's own doc comment.  `table->m_field34` is the
//  same player-slot-table sub-object gamewar3.h/worldframelocalplayer.cpp
//  already name m_field34 - reached here off the raw g_unk6FAB65F4 global
//  the way playertableunitfogrefresh.cpp's own RefreshUnitFog does,
//  because this call site (unlike RefreshUnitFog's) never lazily
//  acquires the table first, only reads it.
//----------------------------------------------------------------------------
struct SPlayerTableField34View
{
    char pad[0x34];
    void* m_field34;
};

void CUnit::RefreshField10C()
{
    SPlayerTableField34View* table = (SPlayerTableField34View*)g_unk6FAB65F4;
    void* field34 = table->m_field34;

    //  The shipped body allocates one scratch dword on its own stack frame
    //  (seeded with a copy of `this`), hands 0x6F29F040 a *pointer* to it
    //  (one stack argument, `retn 4`), then hands 0x6F406AB0 that same slot
    //  back *by value* (also `retn 4`, and no separate push at its own call
    //  site - it reads the dword 0x6F29F040 already left at [esp]).
    void* scratch = this;
    ((SUnitField10CQueryReceiver*)this)->Query(&scratch);
    unsigned short word =
        ((SPlayerTableField34QueryReceiver*)field34)->Finalize(scratch);

    //  A genuine 16-bit store (`mov [esi+10Ch], ax`) - the shipped body
    //  never touches m_int10C's own upper half, so this is written as one
    //  to match rather than a masked 32-bit read-modify-write.
    *(unsigned short*)&m_int10C = word;
}

//----------------------------------------------------------------------------
//  0x6F284DA0.  See unit.h's own doc comment.  `+0x148` is seeded from
//  QuerySpecialSelectionMask() and then walked bit by bit: for every live
//  player index (0..11) whose bit is set in that same +0x148 mask, fetch
//  that player's relation agent (SJassPlayerRelations::EnsureRelationAgent,
//  called twice - once per field read, the same pattern RefreshUnitFog and
//  CJassGameStateMasks::DetectedMask both already establish) and OR its
//  +0x88 and +0xC8 fields into +0x14C.  +0x148 and +0x14C are not named
//  fields in unit.h (raw offsets, the same way unit_save.cpp/unit_load.cpp
//  already read the neighbouring +0x144/+0x148 pair through
//  m_reserved144 - see that field's own comment for why the whole
//  0x144..0x150 run stays unsplit).
//----------------------------------------------------------------------------
void CUnit::RefreshRelationMaskFields()
{
    unsigned int mask = (unsigned short)QuerySpecialSelectionMask();
    *(unsigned int*)((char*)this + 0x148) = mask;
    *(unsigned int*)((char*)this + 0x14C) = mask;

    void* table = g_unk6FAB65F4;

    for (unsigned int i = 0; i < 0xC; ++i)
    {
        SJassPlayerRelations* record = (SJassPlayerRelations*)
            ((IndexedArrayHolder_6F3A1650*)table)->At(i);

        if (*(unsigned int*)((char*)this + 0x148) & (1u << i))
        {
            SJassRelationAgent* agent = record->EnsureRelationAgent();
            *(unsigned int*)((char*)this + 0x14C) |=
                (unsigned short)agent->m_reserved88.QueryField78();

            SJassRelationAgent* agent2 = record->EnsureRelationAgent();
            *(unsigned int*)((char*)this + 0x14C) |=
                (unsigned short)QueryField78At0xC8(agent2);
        }
    }
}
