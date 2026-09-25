//============================================================================
//  0x6F488F20 - SScratchPoolOwner::BorrowScratchRelatedList.  Own
//  translation unit, split out of Agent/poposcl_relatedagentslot21.cpp:
//  that file's own `NIpse::CPoPosCl::Slot21` is this function's only
//  caller anywhere in this dump, and `/Ob2` inlines a single-call-site
//  function straight into its caller when both live in the same TU -
//  losing the shipped `call sub_6F488F20` entirely and inflating Slot21's
//  own frame with this function's locals, exactly the trap
//  `poposcl_relatedagentslot4_teardown.cpp`'s own header comment already
//  documents for an analogous pair in this same class family
//  (CLAUDE.md's own "One translation unit per original module" rule -
//  put a callee in its own TU rather than reaching for
//  `__declspec(noinline)`, which the shipped build never used).
//============================================================================
#include "spatialgrid.h"

//----------------------------------------------------------------------------
//  Borrow the next scratch related-agent list off CTimeSync's own fixed
//  8-slot pool (+0x210, eight `SGridRegistrationPtrArray*` pointers) and
//  clear out whatever it held from its previous borrow first. Returns 0
//  once all eight have been handed out - the cursor at +0x230 only ever
//  counts up, `SField230Owner::DecrementField230`
//  (Misc/misc_trivial_getters.cpp, called at the end of Slot21) is its
//  only release. `this` in ecx, no stack arguments, `retn`.
//----------------------------------------------------------------------------
struct SScratchPoolOwner
{
    SGridRegistrationPtrArray* __thiscall BorrowScratchRelatedList();
};

SGridRegistrationPtrArray*
    __thiscall SScratchPoolOwner::BorrowScratchRelatedList()
{
    unsigned int cursor = *(unsigned int*)((char*)this + 0x230);
    if (cursor == 8)
        return 0;

    SGridRegistrationPtrArray** pool =
        (SGridRegistrationPtrArray**)((char*)this + 0x210);
    SGridRegistrationPtrArray* slot = pool[cursor];

    if (slot->m_count != 0)
        slot->RemoveRange(0, slot->m_count);

    //  A direct `add [this+0x230], 1` in the shipped stream, not a
    //  read-modify-write through the already-live `cursor` local: no
    //  register needs to survive the `RemoveRange` call above this way.
    *(unsigned int*)((char*)this + 0x230) += 1;
    return slot;
}
