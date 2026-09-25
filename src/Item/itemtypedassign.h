//============================================================================
//  A local, type-checked "smart pointer to an item-derived CAgent"
//  reached while resolving a JASS item handle - see
//  jasssetitemposition_native.cpp for the two call sites (both a bare
//  local variable, not a real member of any class this call tree shows).
//  Same shape as CAgentPtr's own converting constructor/AssignChecked
//  (agent.h/agent.cpp): type-check against a fixed FourCC ('item', not a
//  polymorphic root type id), release-old/addref-new, exception-safe
//  construction via __try/__finally.
//============================================================================
#ifndef ITEMTYPEDASSIGN_H
#define ITEMTYPEDASSIGN_H

#include "agent.h"

struct SCheckedItemSlot
{
    //  0x6F038890 - see itemtypedassign.cpp.
    SCheckedItemSlot(CAgent* candidate);

    //  Not a separately reconstructed address - every caller in this call
    //  tree has this inlined directly into its own cleanup code (see
    //  itemhandlemain.cpp's own scope-exit releases), the same way a
    //  short, single-call-site destructor routinely does under /Ob2.
    ~SCheckedItemSlot() { Release(); }

    //  0x6F02A6A0 - type-check candidate against 'item' (GetItemTypeFourCC,
    //  itemhandletable.h), release the old value if it is being replaced,
    //  addref the new one, store, and return `this` - the same shape as
    //  CAgentPtr::AssignChecked, just against a fixed required type
    //  instead of a polymorphic GetAgileRootTypeId().
    SCheckedItemSlot* Assign(CAgent* candidate);

    //  Release-if-set, no vtable-slot zero after - the same shape
    //  CAgentPtr::Release (agent.h) uses for the identical reason (this
    //  object's about to be considered dead either way).
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

#endif
