//============================================================================
//  0x6F41FA10 and 0x6F41FE20 - the two read-side methods of the per-player
//  selection holder, the object every player selection record parks at
//  +0x34 and CItem's slots 98 and 102 reach through
//  (item_selected.cpp, item_selectionrefresh.cpp).
//
//  Both were `mov eax,<address> / jmp eax` redirects, and both are now
//  reconstructed: neither is a subsystem, they are a field read and a
//  three-test compare-and-drop.
//
//  The two class names are the two the rest of src/ already declares them
//  under - `SSelectionRecord` at the slot-98 call site and `SSelectionSet`
//  at the slot-102 one.  They are one shipped class (both methods key off
//  the same +0x1E4 field), but the names are funcmap.py keys and a rename
//  is a new key (CLAUDE.md, "a rename is a new key"), so they are kept and
//  the duplication is documented here rather than papered over.
//
//  Own translation unit: 0x6F41FE20 reaches the drop below with a real
//  `call`, so its callee cannot share a file with it, and the two call
//  sites in item_selected.cpp / item_selectionrefresh.cpp must not inline
//  either of these.
//============================================================================
#include "item.h"

//----------------------------------------------------------------------------
//  The holder.  Only +0x1E4 is touched by anything in this file; the
//  0x1E4 bytes ahead of it are whatever the selection UI keeps there and
//  are not recoverable from these two bodies.
//----------------------------------------------------------------------------
struct SSelectionRecord
{
    CAgent* GetSelectedAgent();

    char    m_reserved00[0x1E4];
    CAgent* m_pSelected;        // +0x1E4
};

struct SSelectionSet
{
    int  Contains(CAgent* agent);

    //  0x6F41FA20 - drop whatever is in +0x1E4: tell it through its own
    //  vtable that it is no longer selected, then release this holder's
    //  counted reference to it.  A real call in the shipped code, so it
    //  lives in its own translation unit (selectionclear.cpp).
    void ClearSelection();

    char    m_reserved00[0x1E4];
    CAgent* m_pSelected;        // +0x1E4
};

//----------------------------------------------------------------------------
//  0x6F41FA10 - the agent this holder currently has selected.
//----------------------------------------------------------------------------
CAgent* SSelectionRecord::GetSelectedAgent()
{
    return m_pSelected;
}

//----------------------------------------------------------------------------
//  0x6F41FE20 - "is this the agent I am holding, and if so let go of it".
//
//  Named `Contains` because that is the funcmap.py key CItem's slot 102
//  already reaches it under, but the shipped body is a *test and drop*, not
//  a pure query: the true branch runs the release above before answering 1.
//  Slot 102 relies on that - it walks all sixteen records precisely so that
//  every holder still pointing at a dying item lets go of it.
//
//  Written as one `&&` chain into a single early exit rather than as two
//  guarded returns: the shipped code falls both failing tests through to
//  one shared `xor eax,eax`.
//----------------------------------------------------------------------------
int SSelectionSet::Contains(CAgent* agent)
{
    if (agent != 0 && agent == m_pSelected)
    {
        ClearSelection();
        return 1;
    }
    return 0;
}
