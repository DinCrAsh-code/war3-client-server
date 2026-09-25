//============================================================================
//  The four location-making natives of the 0x6F3D0xxx..0x6F3D2xxx factory
//  module that are not `Location` itself:
//
//      RectFromLoc        "(Hlocation;Hlocation;)Hrect;"   0x6F3D0BD0
//      GetOrderPointLoc   "()Hlocation;"                   0x6F3D1FF0
//      GetSpellTargetLoc  "()Hlocation;"                   0x6F3D2090
//      GetUnitLoc         "(Hunit;)Hlocation;"             0x6F3D2AE0
//
//  Three of them end in a real __cdecl call to JASS_Location
//  (jassnatives_locationmake.cpp), which is why that native keeps its own
//  translation unit even though the shipped module is the same one: in a
//  single file /Ob2 would be free to inline it and all three call sites
//  would stop being calls.
//============================================================================
#include "jassnatives.h"
#include "jassevents.h"
#include "agenttypedslots.h"
#include "gamebounds.h"
#include "itemhandlemain.h"
#include "itemhandletable.h"
#include "agentregistry.h"
#include "unit.h"
#include "timesyncbounds.h"
#include "game.h"
#include "CFloat.h"

//  0x6F3D03B0 - the `Location` native itself.
int __cdecl JASS_Location(const int* x, const int* y);

//----------------------------------------------------------------------------
//  0x6F3D0BD0 - `RectFromLoc`.  Two locations become the rect that spans
//  them: each point is clamped into the map's playable rectangle first, then
//  the four bounds are picked and handed to a freshly made '+rct' agent.
//
//  The four picks are written as four *pointers* into the two clamped
//  points, not as four values, because that is what the shipped code does -
//  it keeps all four coordinates on the x87 stack across the whole sequence
//  (`fld`/`fld`/`fcom`, then `fxch st(3)`/`fcomp st(2)`/`fcompp`) and only
//  materialises the selected addresses at the end.  Naming four floats
//  instead spills them.
//
//  Which pick goes into which word is the shipped layout, not a slip: the Y
//  pair lands in +0x24/+0x2C and the X pair in +0x28/+0x30, which is exactly
//  the order JASS_GetRectMinX/MaxX read them back out in
//  (jassnatives_rect.cpp).
//
//  Not exact, and cannot be: the scoped SCheckedRectSlot puts the whole body
//  inside the __except_handler4-shaped frame docs/targets/JASS_Location.md
//  records as unreproducible at this toolchain's fixed /GS- /EHs-c-.
//----------------------------------------------------------------------------
int __cdecl JASS_RectFromLoc(int hLocationA, int hLocationB)
{
    SJassLocation* locA = ResolveLocationHandle(hLocationA);
    SJassLocation* locB = ResolveLocationHandle(hLocationB);

    if (!locA || !locB)
        return 0;

    //  Both points are filled y-then-x, which is the order the shipped code
    //  loads and stores them in - the same "second one first" shape
    //  CJassGameState::DefineStartLocation records.
    CGridVec2 a(kCFloatNoInit);
    CGridVec2 b(kCFloatNoInit);

    a.m_y = *(const CFloat*)&locA->m_y;
    a.m_x = *(const CFloat*)&locA->m_x;
    b.m_y = *(const CFloat*)&locB->m_y;
    b.m_x = *(const CFloat*)&locB->m_x;

    ClampToGameBounds(&a);
    ClampToGameBounds(&b);

    //  Each pick names its *own* answer first - `a > b ? &a : &b` for a
    //  max, `a < b ? &a : &b` for a min - which is the one spelling that
    //  loads the two coordinates in the shipped order and reduces to
    //  `fcom` / `fnstsw` / `test ah,5` / `jnp`, with the default address
    //  already in a register and the other arm skipped.  Naming the other
    //  operand first (`b < a ? &a : &b`) loads them the other way round and
    //  costs the `fxch` positions of the two comparisons after it; writing
    //  it as `if (!(x < y)) p = &y;` makes MSVC rewrite the negation and the
    //  test becomes `test ah,41h` / `je`.  The same "name the already
    //  loaded operand first" finding docs/targets/JASS_Location.md records
    //  for QueryWorldHeightAt's max.
    const CFloat* maxX =
        (*(const float*)&a.m_x > *(const float*)&b.m_x) ? &a.m_x : &b.m_x;
    const CFloat* maxY =
        (*(const float*)&a.m_y > *(const float*)&b.m_y) ? &a.m_y : &b.m_y;
    const CFloat* minX =
        (*(const float*)&a.m_x < *(const float*)&b.m_x) ? &a.m_x : &b.m_x;
    const CFloat* minY =
        (*(const float*)&a.m_y < *(const float*)&b.m_y) ? &a.m_y : &b.m_y;

    SCheckedRectSlot held(0);
    held.MakeRect(0, 0, 0);

    ((SJassRect*)held.m_value)->SetBounds(
        SJassBounds(minY->m_bits, minX->m_bits,
                    maxY->m_bits, maxX->m_bits));

    CAgentRegistry* registry = (CAgentRegistry*)g_pItemSlotHost->GetSlotTable();
    return (int)registry->Register((CUnitAgent*)held.m_value, 0);
}

//----------------------------------------------------------------------------
//  0x6F3D1FF0 - `GetOrderPointLoc`.  Two event ids reach the same answer by
//  different routes (jassevents.h); everything else answers zero.  When the
//  route resolves but the event data does not, the native still makes a
//  location - at (0, 0) - rather than returning null, which is the shipped
//  behaviour and the reason both arms share that tail.
//----------------------------------------------------------------------------
int __cdecl JASS_GetOrderPointLoc()
{
    //  One pair per branch, both at function scope: the shipped frame is
    //  sixteen bytes, so the two branches do *not* share their coordinate
    //  slots.  Declared inside the two `case` blocks MSVC overlaps them and
    //  the frame is eight.
    CFloat ownerX(kCFloatNoInit);
    CFloat ownerY(kCFloatNoInit);
    CFloat dataX(kCFloatNoInit);
    CFloat dataY(kCFloatNoInit);

    switch (GetCurrentEventId() + kEventIdBias)
    {
    case 0x80227:
    {
        SOrderEventOwner* owner = (SOrderEventOwner*)GetOrderEventOwner();
        if (!owner)
            return 0;

        CScriptEventData* data = (CScriptEventData*)owner->GetEventData();
        if (!data)
            return JASS_Location(&g_CFloatZero.m_bits, &g_CFloatZero.m_bits);

        return JASS_Location((const int*)data->GetOrderPointX(&ownerX),
                             (const int*)data->GetOrderPointY(&ownerY));
    }

    case 0x8024C:
    {
        CScriptEventData* data = (CScriptEventData*)GetOrderEventData();
        if (!data)
            return JASS_Location(&g_CFloatZero.m_bits, &g_CFloatZero.m_bits);

        return JASS_Location((const int*)data->GetOrderPointX(&dataX),
                             (const int*)data->GetOrderPointY(&dataY));
    }
    }

    return 0;
}

//----------------------------------------------------------------------------
//  0x6F3D2090 - `GetSpellTargetLoc`.  The ability the current spell event is
//  about is staged through a type-checked slot, its handle is re-validated
//  the same way every resolver in this family validates one ('+agl' tag,
//  pending flag clear), and the target point comes back out of two of its own
//  vtable slots.
//
//  Those two slots are dispatched by hand: nothing in this repo establishes a
//  vtable for the ability class, and at +0x3CC / +0x3D0 a guessed one would
//  misplace every slot around them.
//
//  Not exact for the same reason RectFromLoc is not - the scoped slot's SEH
//  frame.
//----------------------------------------------------------------------------
int __cdecl JASS_GetSpellTargetLoc()
{
    typedef CFloat* (__thiscall *GetSpellTargetCoordFn)(void*, CFloat*);

    SCheckedSpellAbilitySlot held((CAgent*)GetSpellEventAbility());

    //  Four separate early returns and no `validated` flag: unlike the
    //  resolvers, whose failure path has a second slot to construct, this
    //  one has nothing to do but leave, so the shipped code branches
    //  straight out at each test.  A flag costs a `sete`/`test al,al` pair
    //  the shipped stream does not have.
    CAgent* ability = held.m_value;
    if (!ability)
        return 0;

    SItemHandleObject* handleObj =
        (SItemHandleObject*)LookupHandle(ability->m_handle,
                                         ability->m_typeTag);
    if (!handleObj)
        return 0;

    //  The `setz`/`neg`/`sbb`/`and` chain the shipped code uses is MSVC's
    //  branchless spelling of this - see jassresolvelocationhandle.cpp.
    SItemHandleObject* tagOk =
        (handleObj->m_kindTag != 0x2B61676C) ? 0 : handleObj;
    if (!tagOk)
        return 0;

    if (tagOk->m_pendingFlag != 0)
        return 0;

    CFloat x(kCFloatNoInit);
    CFloat y(kCFloatNoInit);

    return JASS_Location(
        (const int*)((GetSpellTargetCoordFn)
                     (*(void***)ability)[0x3CC / 4])(ability, &x),
        (const int*)((GetSpellTargetCoordFn)
                     (*(void***)ability)[0x3D0 / 4])(ability, &y));
}

//----------------------------------------------------------------------------
//  0x6F3D2AE0 - `GetUnitLoc`.  The unit's movement sub-object is a CPathRef,
//  so its world position is one SmartPtrToWorldVec3 (CPathRef.cpp) away; the
//  vector is then copied into a second local, which is the returned
//  temporary being copy-constructed into the named one, and the first two of
//  its three components are what the location is made from.
//
//  The null answer is a bare `return 0` and costs no instruction: the pointer
//  it just tested is already the zero in eax.
//----------------------------------------------------------------------------
int __cdecl JASS_GetUnitLoc(int hUnit)
{
    CUnit* unit = ResolveUnitHandle(hUnit);

    if (!unit)
        return 0;

    //  Declared and *assigned*, not initialised: the shipped code reserves
    //  two whole vectors and copies the returned temporary into the named
    //  one word by word, which is what an assignment does and what a
    //  copy-initialisation does not (MSVC constructs that one straight into
    //  the destination and reserves twelve bytes, not twenty-four).  The
    //  default constructor's three zero stores are dead - every word is
    //  overwritten before it is read - and the shipped stream does not have
    //  them either.
    CWorldVec3 pos(kCFloatNoInit);
    pos = ((const CPathRef*)unit->GetHandleObject())->SmartPtrToWorldVec3();

    return JASS_Location((const int*)&pos.m_x, (const int*)&pos.m_y);
}
