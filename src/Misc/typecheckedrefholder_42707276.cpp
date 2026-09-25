//============================================================================
//  createunit-G4 cluster B - a type-checked smart-pointer setter and its
//  one SEH-wrapped caller.  Same overall shape as
//  Unit/unit_fixedtypeidgetter.cpp's STypeCheckedRefHolder::Set (vtable
//  slot 7 = GetAgileTypeId, AgileTypeIsDerivedFrom against a fixed type,
//  COM-style decrement/vtable-slot-0 release on the old value, addref on
//  the accepted one) but its own class: the fixed type here comes from a
//  real out-of-line call to GetFixedAgileTypeId_42707276
//  (fixedagiletypeid_42707276.cpp) rather than a folded immediate, so the
//  two are written independently rather than sharing a struct.
//============================================================================
#include "agiletype.h"   // AgileTypeIsDerivedFrom (0x6F471910)

unsigned int GetFixedAgileTypeId_42707276();   // 0x6F0D2650, own TU

typedef unsigned int (__thiscall *GetAgileTypeIdFn)(void* self);
typedef void (__thiscall *RefDestroyFn)(void* self);

struct STypeCheckedRefHolder42707276
{
    void* __thiscall Set(void* candidate);
    void* __thiscall ResetThenSet(void* candidate);   // 0x6F2811A0

    void* m_ref;   // +0x00
};

//----------------------------------------------------------------------------
//  0x6F275AB0 - `this` (ecx) is the holder, one stack argument (the
//  candidate object), `retn 4`.  Same accept/release/addref shape as
//  STypeCheckedRefHolder::Set: null candidate is rejected outright,
//  otherwise accepted only if its own vtable slot 7 (GetAgileTypeId)
//  answers a type AgileTypeIsDerivedFrom says derives from the fixed
//  0x42707276 id.  Releases whatever the holder used to point at (skipped
//  when the accepted value is unchanged), addrefs the accepted candidate,
//  stores it, and returns `this`.
//----------------------------------------------------------------------------
void* __thiscall STypeCheckedRefHolder42707276::Set(void* candidate)
{
    //  No named local for the vtable-dispatched type, and `accepted` is not
    //  pre-zeroed at the top: unlike Unit/unit_fixedtypeidgetter.cpp's
    //  STypeCheckedRefHolder::Set (whose fixed-type getter is folded into
    //  an immediate, so neither evaluation order nor register lifetime
    //  shows), GetFixedAgileTypeId_42707276 is a real out-of-line call
    //  here (own TU, never inlined cross-TU) - the shipped stream calls it
    //  *before* the vtable dispatch (only reachable from a single call
    //  expression evaluated right-to-left, the second argument first), and
    //  reuses the same register for that call's own result and for
    //  `accepted`'s "rejected" value - only possible when `accepted` has no
    //  separate zero-initialisation to keep alive across the dispatch call.
    void* accepted;
    if (candidate && AgileTypeIsDerivedFrom(
                          ((GetAgileTypeIdFn)(*(void***)candidate)[7])(candidate),
                          GetFixedAgileTypeId_42707276()))
        accepted = candidate;
    else
        accepted = 0;

    void* old = m_ref;
    if (old != accepted)
    {
        if (old != 0 && --*(int*)((char*)old + 4) == 0)
            ((RefDestroyFn)(*(void***)old)[0])(old);

        if (accepted != 0 && candidate != 0)
            ++*(int*)((char*)candidate + 4);

        m_ref = accepted;
    }

    return this;
}

//----------------------------------------------------------------------------
//  0x6F2811A0 - the one caller in this dump: `this` (ecx) is the same
//  holder, one stack argument (the candidate) passed through unchanged,
//  `retn 4`.  Not a plain forward to Set: it zeroes the holder's own
//  m_ref *before* calling Set, so Set's own "release the old value" branch
//  never fires here (`old` reads back 0) - whatever m_ref pointed at is
//  abandoned rather than released, and the accepted candidate (if any) is
//  stored fresh.  Confirmed from the dump's own `mov dword ptr [esi], 0`
//  immediately ahead of the call, where esi is the same pointer the SEH
//  prologue saved from `this` at function entry.
//
//  Behind an `__except_handler4`-shaped SEH frame (the `push -1` /
//  `push offset SEH_...` / `mov eax, large fs:0` / security-cookie
//  prologue and the matching epilogue) this repo's fixed /EHs-c- /GS-
//  build does not emit - the same shape
//  Unit/unit_fixedtypeidgetter.cpp's own 0x6F1A4C40 wrapper is left
//  "pending" over, and the same reason: nothing in this toolchain
//  reproduces the frame, only the call it wraps.
//----------------------------------------------------------------------------
void* __thiscall STypeCheckedRefHolder42707276::ResetThenSet(void* candidate)
{
    m_ref = 0;
    return Set(candidate);
}
