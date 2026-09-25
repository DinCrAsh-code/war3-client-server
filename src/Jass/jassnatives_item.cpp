//============================================================================
//  The JASS `item` natives that ask an item handle one question about the
//  object behind it - three flag writes, its two world coordinates, and who
//  owns it.
//
//  Six entry points from the registration table at 0x6F3D4020, all __cdecl
//  and all one address neighbourhood (0x6F3C4D00-0x6F3C4E4E):
//
//      SetItemDropOnDeath  sub_6F3C4D00   (Hitem;B)V
//      SetItemDroppable    sub_6F3C4D30   (Hitem;B)V
//      SetItemPawnable     sub_6F3C4D60   (Hitem;B)V
//      GetItemX            sub_6F3C4D90   (Hitem;)R
//      GetItemY            sub_6F3C4DD0   (Hitem;)R
//      GetItemPlayer       sub_6F3C4E10   (Hitem;)Hplayer;
//
//  One translation unit: none of the six calls another (each starts from
//  ResolveItemHandle on its own), so there is no inlining hazard to keep
//  them apart, and they are contiguous in the shipped image - 0x30 bytes
//  apart to the byte for the first four, which is what a single module
//  emitted in source order looks like.
//
//  **The three setters are one shape.**  Resolve, refuse on null, then set
//  or clear a single bit of CAgentWar3::m_flags (+0x20, agentwar3.h - the
//  field CWidget and CSelectable used to each declare their own copy of).
//  The shipped layout is two separate `retn`s, not one merged tail: the
//  `or` path returns on the spot and the `and` path falls into the same
//  `retn` the null refusal jumps to.  That is what an early `return;` plus
//  a plain if/else produces; a merged tail would put one `retn` after both
//  arms.
//
//  The bits are 0x80, 0x400 and 0x2000, and none of the three is aliased by
//  anything else in this block of natives: IsItemPawnable (0x6F3C5010) reads
//  `m_flags >> 0xD & 1`, which is this file's SetItemPawnable bit read back,
//  IsItemPowerup (0x6F3C4FD0) reads bit 11 (0x800) and SetItemCharges
//  (0x6F3C5070) writes 0x1000.  Cross-checked against the dump by eye rather
//  than by score: score_one_func.py compares mnemonics only and cannot see a
//  wrong immediate.
//
//  **Why `void` and not the resolved pointer.**  All three shipped bodies
//  leave eax = the resolved object on the success path and eax = 0 on the
//  refusal, and this binary's whole-program optimisation lets a caller use
//  the eax a callee happened to leave behind
//  (docs/notes/implicit-eax-return.md, SCountedOwner::AddOneToBoth).  Here
//  `void` costs nothing: the resolver's own result is already in eax, the
//  bit write is `or dword ptr [eax+20h], ...` straight off it, and nothing
//  overwrites eax before the `retn` - checked in build/jassnatives_item.asm,
//  not assumed.  So the register state on return matches and the return type
//  can stay the `V` the registration table declares.
//
//  **`real` out.**  GetItemX/GetItemY answer in eax as the raw 32-bit CFloat
//  encoding, never through st(0) - jassnatives.h's own convention for the
//  table's `R`, and the reason both return `int` and end in `.m_bits`.  The
//  refusal is `g_CFloatZero`, loaded as a value (`mov eax, dword_6FAAE470`).
//============================================================================
#include "jassnatives.h"
#include "itemhandlemain.h"
#include "agentwar3.h"
#include "agentregistry.h"
#include "game.h"
#include "CFloat.h"

//  CItem's own vtable[0xB8] (index 46, 0x6F2B76B0) - "hand back the
//  handle-bearing sub-object", declared on CItem (item.h) but reached here
//  the way every other reader of this slot reaches it (widget_worldpos.cpp,
//  jasssetitemposition.cpp): through the slot, on a CAgent* the resolver
//  handed back, never as `this->GetHandleObject()`.  Typed as CPathRef*
//  directly - SmartPtrToWorldVec3 reads exactly the {handle, typeTag} pair
//  at +0x08/+0x0C that both CPathRef and SHandleWithType describe.
typedef CPathRef* (__thiscall *GetHandleObjectFn)(void* self);

//  CItem's own vtable[0x64] (index 25, 0x6F2B7770) - the owning player index,
//  which for an item lives in its Position record (item.h).  Same reason for
//  the hand-written dispatch as the slot above.
typedef unsigned int (__thiscall *GetOwningPlayerIndexFn)(void* self);

//  0x6F3A1650 - IndexedArrayHolder_6F3A1650::At, already reconstructed and
//  EXACT (Misc/misc_field_getters.cpp).  Declared locally, the way every
//  other caller of it in this repo does (item_selected.cpp,
//  gameuiplayername.cpp): a second naked redirect at the same address would
//  be two symbols for one function and would cost that EXACT body every call
//  site it has.  The layout has to match those declarations exactly, or MSVC
//  mangles a second `At` the linker never joins to the real one.
struct IndexedArrayHolder_6F3A1650
{
    char pad[0x58];
    int  m_data[1];
    int  At(unsigned int index);
};

//----------------------------------------------------------------------------
//  0x6F3C4D00 - SetItemDropOnDeath.  Bit 0x80.
//----------------------------------------------------------------------------
void __cdecl JASS_SetItemDropOnDeath(int hItem, int flag)
{
    CAgentWar3* item = (CAgentWar3*)ResolveItemHandle(hItem);
    if (!item)
        return;

    if (flag)
        item->m_flags |= 0x80u;
    else
        item->m_flags &= ~0x80u;
}

//----------------------------------------------------------------------------
//  0x6F3C4D30 - SetItemDroppable.  Bit 0x400.
//----------------------------------------------------------------------------
void __cdecl JASS_SetItemDroppable(int hItem, int flag)
{
    CAgentWar3* item = (CAgentWar3*)ResolveItemHandle(hItem);
    if (!item)
        return;

    if (flag)
        item->m_flags |= 0x400u;
    else
        item->m_flags &= ~0x400u;
}

//----------------------------------------------------------------------------
//  0x6F3C4D60 - SetItemPawnable.  Bit 0x2000, the one IsItemPawnable
//  (0x6F3C5010) reads back as `m_flags >> 0xD & 1`.
//----------------------------------------------------------------------------
void __cdecl JASS_SetItemPawnable(int hItem, int flag)
{
    CAgentWar3* item = (CAgentWar3*)ResolveItemHandle(hItem);
    if (!item)
        return;

    if (flag)
        item->m_flags |= 0x2000u;
    else
        item->m_flags &= ~0x2000u;
}

//----------------------------------------------------------------------------
//  0x6F3C4D90 - GetItemX.
//
//  One chained expression, no named local for the SHandleWithType*/CPathRef*
//  the 0xB8 slot hands back: the shipped code pushes SmartPtrToWorldVec3's
//  own 12-byte return buffer (`lea ecx,[esp]` / `push ecx`) *before* the
//  0xB8 call that produces its `this`, and MSVC only emits that ordering
//  when nothing names the intermediate pointer.  Naming it sinks the `lea`
//  past the call.  jasssetitemposition.cpp:246-249 and widget_worldpos.cpp
//  both record the same point at their own copy of this call pair.
//
//  The whole frame is that one buffer - `sub esp, 0Ch` - which is why the
//  vector is copy-initialised straight out of the call (no default
//  construction, no second slot).
//----------------------------------------------------------------------------
int __cdecl JASS_GetItemX(int hItem)
{
    CAgent* item = ResolveItemHandle(hItem);
    if (!item)
        return g_CFloatZero.m_bits;

    CWorldVec3 pos =
        ((GetHandleObjectFn)(*(void***)item)[0xB8 / 4])(item)
            ->SmartPtrToWorldVec3();

    return pos.m_x.m_bits;
}

//----------------------------------------------------------------------------
//  0x6F3C4DD0 - GetItemY.  The same nineteen instructions as GetItemX with
//  one operand changed: the answer is read out of the buffer's second word
//  (`[esp+4]`) instead of its first.
//----------------------------------------------------------------------------
int __cdecl JASS_GetItemY(int hItem)
{
    CAgent* item = ResolveItemHandle(hItem);
    if (!item)
        return g_CFloatZero.m_bits;

    CWorldVec3 pos =
        ((GetHandleObjectFn)(*(void***)item)[0xB8 / 4])(item)
            ->SmartPtrToWorldVec3();

    return pos.m_y.m_bits;
}

//----------------------------------------------------------------------------
//  0x6F3C4E10 - GetItemPlayer.
//
//  Three steps, and the JASS handle it answers with is made by the last one:
//  ask the item which player index owns it (vtable[0x64]), turn that index
//  into the world object's own per-player record (the +0x58 array
//  IndexedArrayHolder_6F3A1650::At indexes), then register *that* record as
//  an agent in the JASS handle registry and hand back the slot token.  So a
//  JASS `player` handle is not stored on the player - it is minted on demand,
//  and CAgentRegistry::Register (agentregistry.h) answers the existing token
//  when the object already has one.
//
//  The registry is reached through CItemSlotSingletonHost::GetSlotTable, not
//  through dword_6FAB65F4 directly - the same split jassnatives.h already
//  records for CJassHandleRegistry::ReleaseObject, and the reason the global
//  is read *twice* here: once as the array holder (kept in esi across the
//  vtable call) and once as the singleton host.  itemhandlemain.h's own note
//  is why the second load belongs to this instruction stream and not to
//  GetSlotTable's: the shipped code loads dword_6FAB65F4 at the call site.
//
//  The refusal is a bare `retn` with eax already zero, which is exactly what
//  `return 0;` off the null test compiles to - the resolver left the null it
//  answered in the register the return value comes back in.
//----------------------------------------------------------------------------
int __cdecl JASS_GetItemPlayer(int hItem)
{
    CAgent* item = ResolveItemHandle(hItem);
    if (!item)
        return 0;

    //  The world object in a named local *before* the vtable[0x64] call, not
    //  chained onto it: the shipped code loads dword_6FAB65F4 into esi at
    //  0x6F3C4E26, one instruction ahead of the `call eax`, and a chained
    //  `((IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4)->At(vcall())` sinks
    //  that load past the call instead (MSVC evaluates the argument first
    //  and reloads the global afterwards, which is legal because the load
    //  had not happened yet).  Naming it makes the value live across the
    //  call, which is a callee-saved register by construction - and that is
    //  where the shipped `push esi`/`pop esi` pair comes from.
    IndexedArrayHolder_6F3A1650* world =
        (IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4;

    CUnitAgent* owner = (CUnitAgent*)world->At(
        ((GetOwningPlayerIndexFn)(*(void***)item)[0x64 / 4])(item));

    //  The registry in a local too, for the mirror-image reason: written as
    //  one expression, MSVC pushes Register's two arguments *before* calling
    //  GetSlotTable (the receiver is the last thing it needs), so `owner`
    //  never has to survive that call and the shipped `mov esi, eax` /
    //  `push esi` pair disappears.  A named local sequences the receiver
    //  ahead of the argument pushes, which is the shipped order.
    CAgentRegistry* registry =
        (CAgentRegistry*)g_pItemSlotHost->GetSlotTable();

    return (int)registry->Register(owner, 0);
}
