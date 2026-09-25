//============================================================================
//  Type-checked "smart pointer to a CAgent-derived object" slots, one per
//  required type, each with its own Assign at its own address.
//
//  All of them are byte-for-byte the same body as SCheckedItemSlot::Assign
//  (0x6F02A6A0, itemtypedassign.h) with one operand changed: which FourCC
//  the candidate's own agile type id is checked against.  The image was
//  linked without /OPT:ICF (agentdefaults.cpp), so identical bodies at
//  distinct addresses stay distinct functions - and they really are distinct
//  C++ types, not one function called several ways, because the required
//  type is a compile-time constant baked into each.
//
//      0x6F029770  '+w3w'  CWidget       SCheckedWidgetSlot
//      0x6F02A560  '+w3u'  CUnit         SCheckedUnitSlot
//      0x6F2C86C0  '+w3s'  CSelectable   SCheckedSelectableSlot
//      0x6F40CC60  'ally'                SCheckedAllySlot
//      0x6F39F800  '+gam'                SCheckedGameSlot
//      0x6F4271E0  '+qum'                SCheckedQumSlot
//      0x6F387730  '+ply'                SCheckedPlayerSlot
//      0x6F3B62E0  '+loc'                SCheckedLocationSlot
//      0x6F3B6F00  '+frc'                SCheckedForceSlot
//      0x6F33EB40  '+tmr'                SCheckedTimerSlot
//      0x6F3B6040  '+flt'                SCheckedFilterSlot
//      0x6F3B55C0  'bExp'                SCheckedBoolExprSlot
//      0x6F3B6260  '+agr'                SCheckedRegionSlot
//      0x6F3B6500  '+rct'                SCheckedRectSlot
//      0x6F085DC0  '+fgm'                SCheckedFogModifierSlot
//      0x6F3B6580  '+trg'                SCheckedTriggerSlot
//      0x6F02B360  '+ord'                SCheckedOrderSlot
//      0x6F3B7300  '+cst'                SCheckedCameraSetupSlot
//      0x6F2F49E0  '+cam'  CCameraWar3   SCheckedCameraSlot
//
//  Eighteen, then, not four.  The last three arrived one per target - the
//  unit-order slot with JASS_SetUnitPosition, the camerasetup slot with
//  CameraSetupGetDestPositionLoc, and the '+cam' slot with
//  JASS_PanCameraToTimedWithZ, which is the only member outside the JASS
//  handle types.  Two arrived with JASS_UnitItemInSlot's own
//  call tree (docs/targets/JASS_UnitItemInSlot.md), which reached the family
//  from the other side - 0x6F39F800 had been dismissed there as a `jmp`
//  thunk into "ninety-odd instructions of registry bookkeeping"; it is
//  forty-two, and it is this body - and the last nine arrived together with
//  the `Location` native (docs/targets/JASS_Location.md), which is what
//  finally identified the whole **JASS handle-resolution layer**: there is
//  one of these per JASS handle type, and one 115-instruction resolver per
//  type built on top of it.
//
//  Every one of the JASS nine also has a **constructor** at its own address,
//  because its resolver holds it as a scoped local rather than as a member:
//
//      0x6F038450  SCheckedUnitSlot::SCheckedUnitSlot(CAgent*)
//      0x6F428520  SCheckedQumSlot::SCheckedQumSlot(CAgent*)
//      0x6F3B8FA0  SCheckedLocationSlot::SCheckedLocationSlot(CAgent*)
//      0x6F388950  SCheckedPlayerSlot::SCheckedPlayerSlot(CAgent*)
//      0x6F3B9900  SCheckedForceSlot::SCheckedForceSlot(CAgent*)
//      0x6F34BEA0  SCheckedTimerSlot::SCheckedTimerSlot(CAgent*)
//      0x6F3B8B00  SCheckedFilterSlot::SCheckedFilterSlot(CAgent*)
//      0x6F3B8C10  SCheckedBoolExprSlot::SCheckedBoolExprSlot(CAgent*)
//      0x6F3B8F40  SCheckedRegionSlot::SCheckedRegionSlot(CAgent*)
//      0x6F3B9180  SCheckedRectSlot::SCheckedRectSlot(CAgent*)
//      0x6F095D70  SCheckedFogModifierSlot::SCheckedFogModifierSlot(CAgent*)
//      0x6F3B91E0  SCheckedTriggerSlot::SCheckedTriggerSlot(CAgent*)
//
//  All three are the same three statements SCheckedItemSlot's constructor
//  (0x6F038890) already is - zero the value, Assign inside a `__try`,
//  Release in a `__finally` guarded by AbnormalTermination() - and all
//  score the same 11 of 43 it does, for the same unreproducible
//  __except_handler4-shaped frame.  A type that has one also needs a
//  destructor and a Release(), and those are inlined into every caller's
//  own scope-exit code rather than being separate addresses.
//
//  The shape, and why it is spelled the way it is, is documented once in
//  itemtypedassign.cpp; the two things worth repeating here are that the
//  type-id getter is called *before* the candidate's own virtual slot 7
//  because MSVC evaluates AgileTypeIsDerivedFrom's arguments right to left,
//  and that the addref is nested inside its own null test (`if (checked) {
//  if (candidate) ... }`) because the shipped code tests both registers -
//  an inlined addref helper's own guard inside the assign's guard.
//
//  Each gets its own translation unit: separate modules, and nothing here
//  may inline into anything else.
//============================================================================
#ifndef AGENTTYPEDSLOTS_H
#define AGENTTYPEDSLOTS_H

#include "agent.h"

struct SCheckedWidgetSlot
{
    //  0x6F0382B0 - see checkedwidgetslot_ctor.cpp.  CUnit::CUnit()
    //  (unit_ctor.cpp) holds one of these as a member (m_pRef2C4,
    //  constructed in place with a null candidate), which is what
    //  surfaced this address - every other existing caller of this
    //  struct casts raw storage to `(SCheckedWidgetSlot*)` and calls
    //  Assign directly (widgetpathablerefread.cpp), so adding a real
    //  constructor/destructor here does not change any of their codegen.
    SCheckedWidgetSlot(CAgent* candidate);

    ~SCheckedWidgetSlot() { Release(); }

    //  0x6F029770
    SCheckedWidgetSlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

struct SCheckedUnitSlot
{
    //  0x6F038450 - see unittypedassign_ctor.cpp.
    SCheckedUnitSlot(CAgent* candidate);

    //  This class *is* the binary's `CAgentPtr<CUnit>` - the growable array
    //  of them (Containers/unitagentptrarray.cpp) tags its Storm calls
    //  ".?AV?$CAgentPtr@VCUnit@@@@" - so it has a refcounting copy
    //  constructor in the shipped source, and 0x6F09F890's move loop is it
    //  inlined.  It is deliberately *not* declared here: MSVC gives a copy
    //  constructor the same readable name as the converting one
    //  (`SCheckedUnitSlot::SCheckedUnitSlot`), the pipeline keys a
    //  generated stream on that name, and the two would collide - which
    //  cost 0x6F038450 thirteen instructions across 28 dumps when it was
    //  tried.  The move loop is written out in that array's own explicit
    //  specialisation of SetAlloc instead.

    //  Inlined into every caller's own scope-exit code; no address.
    ~SCheckedUnitSlot() { Release(); }

    //  0x6F02A560
    SCheckedUnitSlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  The '+gam' one.  Assign only: its single caller
//  (CItemSlotSingletonHost::GetSlotTable, itemslotsingleton.cpp) assigns
//  into a slot that already exists, so no constructor was ever emitted.
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//  The '+ply' one (0x6F387730, playertypedassign.cpp).  Assign only; the
//  unit factory writes into a slot that already exists.
//----------------------------------------------------------------------------
struct SCheckedPlayerSlot
{
    //  0x6F388950 - see checkedplayerslot.cpp.  It was "Assign only; the
    //  unit factory writes into a slot that already exists" until
    //  ResolvePlayerHandle stopped being a thunk and turned out to hold one
    //  as a local.
    //
    //  The `= 0` default was added for CGameWar3's own +0x58 array of 16 of
    //  these (Game/gamewar3.cpp): the shipped constructor there default-
    //  constructs the whole array through the compiler's own
    //  `eh vector constructor iterator', which needs a zero-argument way to
    //  build each element, and the array's own per-element wrapper
    //  (0x6F3A1A30) is exactly `push 0; call 0x6F388950; retn` - i.e. the
    //  shipped source really does call this with a defaulted 0, not a
    //  second overload.  A default argument does not touch any existing
    //  explicit-argument call site's own codegen.
    SCheckedPlayerSlot(CAgent* candidate = 0);

    ~SCheckedPlayerSlot() { Release(); }

    //  0x6F387730
    SCheckedPlayerSlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

struct SCheckedGameSlot
{
    //  0x6F39F800
    SCheckedGameSlot* Assign(CAgent* candidate);
    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  The '+qum' one, reached from the agent registry's own singleton finish
//  step (0x6F42C210, agentregistrysingleton.cpp).
//----------------------------------------------------------------------------
struct SCheckedQumSlot
{
    //  0x6F428520 - see checkedqumslot.cpp.
    SCheckedQumSlot(CAgent* candidate);

    ~SCheckedQumSlot() { Release(); }

    //  0x6F4271E0
    SCheckedQumSlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  The '+loc' one, the JASS `location` object's own slot: the only local
//  the `Location` native (0x6F3D03B0, jassnatives_locationmake.cpp) holds
//  the freshly made agent in between the factory and the registry.
//
//  Its Release() is spelled `-= 1` and then a separate `== 0` test rather
//  than the `--x == 0` of the four above, because that is what the shipped
//  destructor in JASS_Location is: `add [eax+4],-1` / `mov ecx,eax` /
//  `add eax,4` / `cmp dword ptr [eax],0` / `jnz` re-reads the field it has
//  just written instead of branching on the decrement's own flags, and only
//  the two-statement spelling produces that re-read - the same spelling
//  CWidget::ClearPlayerMaskBits (widget_slot68.cpp) writes out by hand for
//  the identical shipped shape.
//----------------------------------------------------------------------------
struct SCheckedLocationSlot
{
    //  0x6F3B8FA0 - see checkedlocationslot.cpp.
    SCheckedLocationSlot(CAgent* candidate);

    ~SCheckedLocationSlot() { Release(); }

    //  0x6F3B62E0 - see locationtypedassign.cpp.
    SCheckedLocationSlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

struct SCheckedSelectableSlot
{
    //  0x6F2C9670 - the same three-statement constructor the twelve JASS
    //  slots above have, reconstructed in GameUI/gameuiselectableslot.cpp.
    //  The unit tip holds one of these as a *member* rather than as a
    //  scoped local, which is why its Release() is a separate address the
    //  frame's own unwind trampoline calls (0x6F2C8720) instead of being
    //  inlined into a caller's scope exit.
    SCheckedSelectableSlot(CAgent* candidate);

    //  0x6F2C86C0
    SCheckedSelectableSlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

struct SCheckedAllySlot
{
    //  0x6F410A90 - see checkedallyslot_ctor.cpp.  Found by
    //  CPlayerWar3::CPlayerWar3() (playerwar3_ctor.cpp): zero m_value, then
    //  Assign(candidate) - the same "Assign only" family's converting
    //  constructor every sibling with one has, just not previously known to
    //  exist for this member (the table's header comment above still says
    //  "Assign only" - the second, scoped-local use CPlayerWar3 makes of it
    //  is what surfaced the constructor address).
    SCheckedAllySlot(CAgent* candidate);

    //  0x6F40CC60
    SCheckedAllySlot* Assign(CAgent* candidate);
    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  The eight remaining JASS handle types, one struct each.  All eight are
//  the same three members SCheckedLocationSlot has and all eight spell
//  Release() the same way it does - `-= 1` and then a separate `== 0` test,
//  because that is what their resolvers' own inlined scope-exit code is
//  (`add [eax+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp dword ptr [eax],0`
//  / `jnz` re-reads the field it has just written).
//----------------------------------------------------------------------------

struct SCheckedForceSlot
{
    //  0x6F3B9900 - see checkedforceslot.cpp.
    SCheckedForceSlot(CAgent* candidate);

    ~SCheckedForceSlot() { Release(); }

    //  0x6F3B6F00 - '+frc'.
    SCheckedForceSlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

struct SCheckedTimerSlot
{
    //  0x6F34BEA0 - see checkedtimerslot.cpp.
    SCheckedTimerSlot(CAgent* candidate);

    ~SCheckedTimerSlot() { Release(); }

    //  0x6F33EB40 - '+tmr'.
    SCheckedTimerSlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

struct SCheckedFilterSlot
{
    //  0x6F3B8B00 - see checkedfilterslot.cpp.
    SCheckedFilterSlot(CAgent* candidate);

    ~SCheckedFilterSlot() { Release(); }

    //  0x6F3B6040 - '+flt'.
    SCheckedFilterSlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

struct SCheckedBoolExprSlot
{
    //  0x6F3B8C10 - see checkedboolexprslot.cpp.
    SCheckedBoolExprSlot(CAgent* candidate);

    ~SCheckedBoolExprSlot() { Release(); }

    //  0x6F3B55C0 - 'bExp'.
    SCheckedBoolExprSlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

struct SCheckedRegionSlot
{
    //  0x6F3B8F40 - see checkedregionslot.cpp.
    SCheckedRegionSlot(CAgent* candidate);

    ~SCheckedRegionSlot() { Release(); }

    //  0x6F3B6260 - '+agr'.
    SCheckedRegionSlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

struct SCheckedRectSlot
{
    //  0x6F3B9180 - see checkedrectslot.cpp.
    SCheckedRectSlot(CAgent* candidate);

    ~SCheckedRectSlot() { Release(); }

    //  0x6F3B6500 - '+rct'.
    SCheckedRectSlot* Assign(CAgent* candidate);

    //  0x6F3CF490 - make a fresh '+rct' agent out of the type's own pool
    //  and Assign it into this slot, returning `this`.  `retn 0Ch`; only
    //  the last of the three arguments is read.  See jassrectmake.cpp.
    SCheckedRectSlot* MakeRect(int unused0, int unused1, int kind);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  Three more of the same family, added with the JASS *event* natives.  The
//  tag each one checks is read the same way every other entry above was -
//  the constructor calls its Assign, and the Assign calls one
//  `mov eax,<imm32>` / `retn` type-id leaf:
//
//      0x6F3BA320 ctor -> 0x6F3B7560 Assign -> 0x6F4327D0  '+evt'
//      0x6F27C350 ctor -> 0x6F271E90 Assign -> 0x6F431EA0  'wscd'
//      0x6F0EC480 ctor -> 0x6F0E0AC0 Assign -> 0x6F028B10  'AAsm'
//
//  '+evt' is the JASS `event` type, the eleventh member of the '+'-prefixed
//  family jassagiletypeids.cpp catalogues.  'wscd' is CScriptEventData's own
//  tag: its vftable (??_7CScriptEventData@@6B@) puts a body at slot 7 that
//  is literally `mov eax, 77736364h` / `retn`, so the class name is the
//  binary's, not a guess.  'AAsm' is a third tag in the ability module,
//  next door to CAbility's own 'abil' (0x6F021200) and CAbilitySpell's
//  'AAsp' (0x6F028F60); every JASS native that stages an object through the
//  slot below is a `GetSpell*` one, which is what names it.
//----------------------------------------------------------------------------
struct SCheckedEventSlot
{
    //  0x6F3BA320 - see checkedeventslot.cpp.
    SCheckedEventSlot(CAgent* candidate);

    ~SCheckedEventSlot() { Release(); }

    //  0x6F3B7560 - '+evt'.
    SCheckedEventSlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

struct SCheckedEventDataSlot
{
    //  0x6F27C350 - see checkedeventdataslot.cpp.
    SCheckedEventDataSlot(CAgent* candidate);

    ~SCheckedEventDataSlot() { Release(); }

    //  0x6F271E90 - 'wscd'.
    SCheckedEventDataSlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

struct SCheckedSpellAbilitySlot
{
    //  0x6F0EC480 - see checkedspellabilityslot.cpp.
    SCheckedSpellAbilitySlot(CAgent* candidate);

    ~SCheckedSpellAbilitySlot() { Release(); }

    //  0x6F0E0AC0 - 'AAsm'.
    SCheckedSpellAbilitySlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

struct SCheckedFogModifierSlot
{
    //  0x6F095D70 - see checkedfogmodifierslot.cpp.
    SCheckedFogModifierSlot(CAgent* candidate);

    ~SCheckedFogModifierSlot() { Release(); }

    //  0x6F085DC0 - '+fgm'.
    SCheckedFogModifierSlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  The unit-order one, added with JASS_SetUnitPosition (0x6F3C5A50), whose
//  stop order is made through it.  Its type-id leaf is GetOrderAgileTypeId
//  (0x6F2712B0, '+ord', misc_rawcode_getters.cpp) - not an inference: the
//  Assign at 0x6F02B360 is byte-for-byte the same 0x5C-byte body as
//  SCheckedCameraSetupSlot::Assign at 0x6F3B7300 and its one `call` at
//  +0x0D resolves to exactly that leaf.
//----------------------------------------------------------------------------
struct SCheckedOrderSlot
{
    //  0x6F03B620 - see checkedorderslot.cpp.
    SCheckedOrderSlot(CAgent* candidate);

    ~SCheckedOrderSlot() { Release(); }

    //  0x6F02B360 - '+ord'.
    SCheckedOrderSlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  The '+cam' one, the game camera's own slot.  Its type-id getter is
//  0x6F301E10, already reconstructed in trivialaccessors_02_03.cpp as the
//  constant 0x2B63616D - "+cam" read most significant byte first - and the
//  cursor frame CGameUI keeps at +0x3BC holds one at its own +0x19C
//  (gameui.h).  The only member of the family outside the JASS handle
//  types.
//----------------------------------------------------------------------------
struct SCheckedCameraSlot
{
    //  0x6F2F77A0 - see checkedcameraslot.cpp.
    SCheckedCameraSlot(CAgent* candidate);

    ~SCheckedCameraSlot() { Release(); }

    //  0x6F2F49E0 - '+cam'.
    SCheckedCameraSlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  The JASS `camerasetup` one, added with CameraSetupGetDestPositionLoc
//  (0x6F3D3E40).  Its type-id leaf is 0x6F3DA570 - `mov eax, 2B637374h`,
//  '+cst' read most significant byte first, the same '+'-prefixed family
//  every other agile type in this table belongs to, and the only member of
//  it whose three letters spell an abbreviation rather than a word.
//----------------------------------------------------------------------------
struct SCheckedCameraSetupSlot
{
    //  0x6F3B9C00 - see checkedcamerasetupslot.cpp.
    SCheckedCameraSetupSlot(CAgent* candidate);

    ~SCheckedCameraSetupSlot() { Release(); }

    //  0x6F3B7300 - '+cst'.
    SCheckedCameraSetupSlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

struct SCheckedTriggerSlot
{
    //  0x6F3B91E0 - see checkedtriggerslot.cpp.
    SCheckedTriggerSlot(CAgent* candidate);

    ~SCheckedTriggerSlot() { Release(); }

    //  0x6F3B6580 - '+trg'.
    SCheckedTriggerSlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  The '+w3d' one - CDestructable's.  Unlike every sibling above it does not
//  bake the required type id in as an immediate: the shipped body *calls*
//  the free FourCC getter at 0x6F266160 (misc_rawcode_getters.cpp) for it,
//  which is what makes 0x6F010420 a distinguishable function rather than a
//  fifteenth copy of the same three instructions.
//
//  It is the only member of the family whose scoped constructor is reached
//  from the UI side rather than from a JASS resolver: CGameUI's selection
//  sweep and its visual-placement pass each hold one (gameuivisualplacement
//  .cpp, gameuiselectionoverlay.cpp), which is where batch A first met it
//  as the thunk it called SCheckedAgentSlot.
//----------------------------------------------------------------------------
struct SCheckedDestructableSlot
{
    //  0x6F012CF0 - see checkeddestructableslot.cpp.  It *is* this class's
    //  converting constructor - same three statements, same __try/__finally
    //  as every sibling's - but it is spelled as an ordinary member here on
    //  purpose.  Its callers (0x6F34D760, 0x6F37BAB0, 0x6F35B380) treat the
    //  slot as raw storage they hand to it: none of them has the null test
    //  a placement `new (out)` would put in front of a constructor, and
    //  none of them runs a scope-exit destructor a named local would get.
    //  Spelling it `Construct` is what keeps this address to one C++ name
    //  without changing four already-matching call sites into shapes the
    //  shipped stream does not have.
    //  Returns `this`, the way the shipped constructor leaves it in eax.
    SCheckedDestructableSlot* Construct(CAgent* candidate);

    //  0x6F010420 - see destructabletypedassign.cpp.
    SCheckedDestructableSlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};



struct SCheckedOrderPointSlot
{
    //  0x6F03C110 - see checkedorderpointslot.cpp.
    SCheckedOrderPointSlot(CAgent* candidate);

    ~SCheckedOrderPointSlot() { Release(); }

    //  0x6F02B760 - 'ord.'.
    SCheckedOrderPointSlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  The three order/task slots the *targeted* order path adds.  Each one
//  gets its required type id from a `call` rather than from an immediate,
//  the way SCheckedDestructableSlot does, and each has a scoped
//  constructor of its own because its one caller holds it as a local:
//
//      0x6F0388F0 ctor -> 0x6F02A720 Assign -> 0x6F271430  'ordt'
//      0x6F03C110 ctor -> 0x6F02B760 Assign -> 0x6F271330  'ord.'
//      0x6F27EFE0 ctor -> 0x6F2741D0 Assign -> 0x6F271940  'tskA'
//
//  'ordt' and 'ord.' are COrderTarget's and COrderPoint's - two of the five
//  order classes the binary's own RTTI names (COrder, COrderPoint,
//  COrderPoint2, COrderTarget, COrderTarget2), and the two whose factories
//  (0x6F294D40 and 0x6F294B30) are what a JASS target order and a JASS
//  point order are actually made of.  'tskA' is CTaskAction's, the one
//  member of the parallel CTask* family this tree reaches - CUnit::
//  FinishCurrentOrder makes one and hands it the 0xD0162 order id.
//  unitorder.h has the whole tag table and how it was read.
//----------------------------------------------------------------------------
struct SCheckedOrderTargetSlot
{
    //  0x6F0388F0 - see checkedordertargetslot.cpp.
    SCheckedOrderTargetSlot(CAgent* candidate);

    ~SCheckedOrderTargetSlot() { Release(); }

    //  0x6F02A720 - 'ordt'.
    SCheckedOrderTargetSlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};


struct SCheckedTaskActionSlot
{
    //  0x6F27EFE0 - see checkedtaskactionslot.cpp.
    SCheckedTaskActionSlot(CAgent* candidate);

    ~SCheckedTaskActionSlot() { Release(); }

    //  0x6F2741D0 - 'tskA'.
    SCheckedTaskActionSlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//  0x6F0DF440 - SCheckedTaskSlot::Assign, from the CUnit vtable closure walk
//  (cunit_agent2_worklist.md).  Checked against 'task' (GetTaskAgileTypeId,
//  0x6F2717C0, misc_rawcode_getters.cpp) - CTask's own base agile type id,
//  not one of the CTask-derived leaves like CTaskAction ('tskA') above.
//  0x6F0E7470 - the converting constructor a later BFS pass found: the same
//  "zero the value, Assign inside a __try, Release in a __finally guarded
//  by AbnormalTermination()" three statements as every other scoped-local
//  member of this family (checkedtaskslot_assign.cpp).
struct SCheckedTaskSlot
{
    SCheckedTaskSlot(CAgent* candidate);

    //  0x6F0DF440
    SCheckedTaskSlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  --- cunit_agent7 batch, CUnit closure BFS depth 1-2 ---
//
//  Three plain "Assign only" members (own required type id fetched from a
//  private `mov eax,<imm32>/retn` leaf rather than baked in as an
//  immediate, the same shape SCheckedWidgetSlot's own
//  GetWidgetAgileTypeId() call has).  None of the three tags decodes to a
//  recognisable English word the way 'ally'/'+ord' do, so each keeps its
//  literal FourCC name per this file's own rule for a tag nothing in any
//  dump resolves further.
//----------------------------------------------------------------------------
struct SCheckedAAbtSlot
{
    //  0x6F029770-style "Assign only" member - no converting constructor
    //  reachable from this batch's own closure.
    //  0x6F0755F0 - 'AAbt' (via a private leaf at 0x6F0241E0).  See
    //  checkedaabtslot.cpp.
    SCheckedAAbtSlot* Assign(CAgent* candidate);
    CAgent* m_value;
};

struct SCheckedAHbuSlot
{
    //  0x6F2754B0 - 'AHbu' (via a leaf at 0x6F0D9020, cunit_agent5's own
    //  worklist address - declared, not defined, here).  See
    //  checkedahbuslot.cpp.
    SCheckedAHbuSlot* Assign(CAgent* candidate);
    CAgent* m_value;
};

struct SCheckedAseiSlot
{
    //  0x6F2758B0 - 'Asei', via GetTypeTag_6F028840 (already reconstructed
    //  and EXACT, Misc/trivialaccessors_02_03.cpp).  See checkedaseislot.cpp.
    SCheckedAseiSlot* Assign(CAgent* candidate);
    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  Three converting constructors reached from CUnit's own 78-instruction
//  "GetXxxAbility"-shaped accessors (Unit/cunit_agent7_getability_*.cpp).
//  Same three-statement shape and same unreproducible __except_handler4
//  frame as every other constructor in this file (see itemtypedassign.cpp).
//  Each one's own Assign belongs to a different agent's worklist in this
//  parallel batch, so it is declared here (needed to compile the
//  constructor's call) but deliberately not defined in this batch:
//
//      0x6F092A00 ctor -> 0x6F084EF0 Assign -> 0x6F0C4D10 'Aatk' (cunit_agent6)
//      0x6F226520 ctor -> 0x6F219380 Assign -> 0x6F0DB030 'AInv' (cunit_agent6)
//      0x6F280EA0 ctor -> 0x6F2756B0 Assign -> 0x6F0D9220 'AGbu' (cunit_agent3)
//
//  'Aatk'/'AInv' are recognisable WC3 ability rawcodes (basic Attack,
//  Inventory); 'AGbu' is not, so that one struct keeps a literal name.
//----------------------------------------------------------------------------
struct SCheckedAttackAbilitySlot
{
    //  0x6F092A00 - see checkedattackabilityslot.cpp.
    SCheckedAttackAbilitySlot(CAgent* candidate);

    ~SCheckedAttackAbilitySlot() { Release(); }

    //  0x6F084EF0 - 'Aatk'.  cunit_agent6's own worklist address; declared
    //  only, not defined here.
    SCheckedAttackAbilitySlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

struct SCheckedInventoryAbilitySlot
{
    //  0x6F226520 - see checkedinventoryabilityslot.cpp.
    SCheckedInventoryAbilitySlot(CAgent* candidate);

    ~SCheckedInventoryAbilitySlot() { Release(); }

    //  0x6F219380 - 'AInv'.  cunit_agent6's own worklist address; declared
    //  only, not defined here.
    SCheckedInventoryAbilitySlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

struct SCheckedAGbuAbilitySlot
{
    //  0x6F280EA0 - see checkedagbuabilityslot.cpp.
    SCheckedAGbuAbilitySlot(CAgent* candidate);

    ~SCheckedAGbuAbilitySlot() { Release(); }

    //  0x6F2756B0 - 'AGbu'.  cunit_agent3's own worklist address; declared
    //  only, not defined here.
    SCheckedAGbuAbilitySlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  0x6F2757B0 - 'Aque'.  Reached from CUnit's slot107 closure (per-template
//  initialiser, 0x6F2A0E30).  "Assign only": no ctor/dtor for this tag turns
//  up in this session's own slice, and no SEH frame around this address
//  either - unlike every ctor in this file, Assign itself never opens one,
//  so this one really does score close to EXACT rather than being capped by
//  the unreproducible frame.  See checkedaqueslot.cpp.
//----------------------------------------------------------------------------
struct SCheckedAqueSlot
{
    //  0x6F2757B0
    SCheckedAqueSlot* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  The '+tex' one - JASS `ExecuteFunc`'s own call-frame local
//  (Jass/jassruntimeexecutefunc.cpp).  Checked against the exact same
//  floating-text-agent tag GameUI/gameuiagenttypetag.cpp's
//  GetTextAgentTypeId() already names (0x6F443C60, '+tex') - re-used here
//  rather than a distinct JASS-specific type, per this session's own read of
//  the disassembly (not a guess: the constructor's `[esi]=0` and the
//  Assign's virtual-slot-7 check both go through this exact leaf).  Why
//  `ExecuteFunc` needs a floating-text-agent-typed scratch slot rather than
//  something JASS-specific is not established from this closure - see
//  docs/notes/jass-runtime-execute-function.md.
//
//  Same three-statement __try/__finally constructor shape as every sibling
//  above (score capped by the same unreproducible __except_handler4 frame),
//  and the same MakeRect-shaped "build one fresh agent and Assign it in"
//  helper SCheckedRectSlot has (jassrectmake.cpp).
//----------------------------------------------------------------------------
struct SCheckedTextAgentSlot
{
    //  0x6F3A1D00 - see checkedtextagentslot.cpp.
    SCheckedTextAgentSlot(CAgent* candidate);

    ~SCheckedTextAgentSlot() { Release(); }

    //  0x6F39FF70 - '+tex'.
    SCheckedTextAgentSlot* Assign(CAgent* candidate);

    //  0x6F3D0310 - `retn 0xC`.  Build a fresh '+tex' agent out of the
    //  type's own pool and Assign it into this slot, returning `this` - see
    //  jasstextagentmake.cpp.  Only `kind` is read (the other two arguments
    //  are never touched, the same as SCheckedRectSlot::MakeRect); the one
    //  real caller (ExecuteFunc) always passes 0.
    SCheckedTextAgentSlot* MakeTextAgent(int unused0, int unused1, int kind);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

#endif
