//============================================================================
//  0x6F07AA20 / 0x6F07B3E0 - a real slot-42 (SetFlagBit8, unit_flagbit8.cpp)
//  sibling pair reached from CUnit's slot107 closure: another per-ability
//  flag-bit toggle, this one for the 'Arsc' ability rather than SetFlagBit8's
//  own 'Avul', and shaped around a *descriptor* whose own +0x30 byte gives
//  which bit of the ability's +0x6C flags word to touch, rather than a
//  single fixed bit.
//
//  0x6F07AA20 is the exact "GetXxxAbility" family shape
//  (checkedslot_agent7_batch_slot107.h's own header comment) - look the
//  'Arsc' ability up via CUnit's own agile-type registry query, or make one
//  if it does not exist yet, and hand back the type-checked agent.
//
//  0x6F07B3E0 (`retn 8`, ecx = the unit) reads FindAbility('Arsc', ...) the
//  same way SetFlagBit8 does:
//    - not found, on requested: make/attach the ability via 0x6F07AA20 and
//      0x6F1DB760, then OR the descriptor's own bit into the new ability's
//      +0x6C flags and notify the unit's ability-list listeners;
//    - not found, on not requested: nothing to do;
//    - found, on requested: nothing to do (the bit is assumed already set -
//      unlike SetFlagBit8, this sibling never removes the whole ability);
//    - found, on not requested: clear just the descriptor's own bit out of
//      the existing ability's +0x6C flags, leaving the ability itself in
//      place.
//============================================================================
#include "unit.h"
#include "checkedarscslot.h"
#include "agiletype.h"
#include "game.h"
#include "widgetagentquery.h"

struct SMadeAgent_Arsc
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

//----------------------------------------------------------------------------
//  0x6F07AA20 - GetOrCreateAbilityAgent_Arsc.  Not a CUnit member: the
//  shipped body never touches ecx at all (same as every sibling in
//  checkedslot_agent7_batch_slot107.h), and its one caller (0x6F07B3E0)
//  calls it with no `this` set up either.
//----------------------------------------------------------------------------
CAgent* GetOrCreateAbilityAgent_Arsc()
{
    SLocalAbilitySlot_Arsc temp(0);
    unsigned int rawcode = GetFourCC_6F1D3160();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D3160(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Arsc*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F1DB760 (`retn 8`) - attach the freshly made/found 'Arsc' agent to
//  the unit under a fixed tag, through the ability's own +0x70 handle
//  sub-object (the same shape 0x6F001EC0's own `[eax+70h]` field reaches
//  elsewhere in this family) and a private finisher (0x6F052AF0, not
//  reconstructed - below this slice's own BFS cut).
//----------------------------------------------------------------------------
struct SArscAbility
{
    void AttachToUnit(class CUnit* unit, const void* tag);
};

#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

ADDR_THUNK(void SArscAbility::AttachToUnit(CUnit* unit, const void* tag), 0x6F1DB760)

#undef ADDR_THUNK

//  unk_6FAAF338 - IDA could not resolve this to printable text (a tag/name
//  argument, the way lazysingleton.cpp's own unk_6F876330 is); given a real
//  local definition, since the injection binder only binds a
//  dword_/byte_/word_/flt_/dbl_/qword_-prefixed funcmap.py global.
static const char g_tag_unk6FAAF338[] = "unk_6FAAF338";

//  SUnitAbilityHost - the same class name and member signatures every
//  other caller of these two already declares (unit_abilitygrantflag.cpp
//  and friends for FindAbility, unit_flagbit8thunks.cpp for
//  NotifyAbilityListChanged) - declared, not defined, here.  A *second*
//  naked definition of either at the same address under a different
//  struct name (tried first, then reverted) is exactly the "one shipped
//  address, two C++ names" defect docs/notes/thunk-removal-pass.md and
//  unit_findability.cpp's own header warn about: unit_herothunks.cpp used
//  to carry a third FindAbility thunk for the same reason and was fixed to
//  stop, which this file must not undo. Reusing the class name lets the
//  linker bind both members to the one real definition
//  (SUnitAbilityHost::FindAbility's own body, unit_findability.cpp) and
//  the one existing thunk (SUnitAbilityHost::NotifyAbilityListChanged,
//  unit_flagbit8thunks.cpp) instead of adding fresh, competing bodies.
struct SUnitAbilityHost
{
    void* FindAbility(int rawcode, int a, int b, int c, int d);
    void  NotifyAbilityListChanged(void* manager);
};

//  A local descriptor carrying only the one field this slot reads: which
//  bit of the ability's own +0x6C flags word this particular caller owns.
//  The struct's real size/other members are not established from this call
//  tree - only offset 0x30 is ever touched.
struct SFlagBitDescriptor
{
    char          m_reserved00[0x30];
    unsigned char m_bitIndex;   // +0x30
};

//  A local overlay onto the CAgent this slot writes into: +0x6C is not one
//  of CAgent's own named fields (agent.h declares none - every reader casts
//  a raw overlay the way this family already does for SMadeAgent_*), so a
//  small struct just for the one touched dword is the same technique.
struct SAbilityFlagsOverlay
{
    char         m_reserved00[0x6C];
    unsigned int m_flags6C;   // +0x6C
};

//----------------------------------------------------------------------------
//  0x6F07B3E0 - see this file's own header comment.
//----------------------------------------------------------------------------
void CUnit::SetAbilityFlagBit_Arsc(void* descArg, int on)
{
    SFlagBitDescriptor* desc = (SFlagBitDescriptor*)descArg;
    //  0x41727363 ('Arsc') pushed as a literal here rather than through
    //  GetFourCC_6F1D3160() - unlike 0x6F07AA20's own call to that leaf,
    //  this call site's `push 1098019683h` is a literal immediate in the
    //  dump, the same inlined-tiny-getter shape SetFlagBit8's own
    //  GetFourCC_6F10FA90() call folds to (unit_flagbit8.cpp).
    void* ability = ((SUnitAbilityHost*)this)
                        ->FindAbility(0x41727363, 0, 0, 1, 1);

    if (ability == 0)
    {
        if (!on)
            return;

        CAgent* made = GetOrCreateAbilityAgent_Arsc();
        ((SArscAbility*)made)->AttachToUnit(this, g_tag_unk6FAAF338);

        unsigned int bit = 1u << desc->m_bitIndex;
        ((SAbilityFlagsOverlay*)made)->m_flags6C |= bit;

        ((SUnitAbilityHost*)this)->NotifyAbilityListChanged(made);
        return;
    }

    if (on)
        return;

    unsigned int bit = 1u << desc->m_bitIndex;
    ((SAbilityFlagsOverlay*)ability)->m_flags6C &= ~bit;
}
