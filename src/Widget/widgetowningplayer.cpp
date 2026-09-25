//============================================================================
//  0x6F266A40 - CWidget vtable[0xEC] (index 59): which player owns this
//  widget.
//
//  Two instructions: `or eax, 0FFFFFFFFh` / `retn`.  A plain widget - and a
//  plain item, which shares this slot unchanged - has **no owning player**,
//  and only CUnit overrides it with a real answer.
//
//  That closes a loose end in NotifyWidgetVisibilityChanged (0x6F2AD7E0),
//  which clears `1 << owner` out of its notify mask when this returns a
//  non-negative index: on a widget or an item that branch is dead, because
//  this always returns -1.  It is reached only through the CUnit override.
//
//  `or eax,-1` rather than `mov eax,-1` is MSVC's own two-byte encoding for
//  the constant; `return -1;` is what produces it.
//============================================================================
#include "widget.h"
#include "mdp_demo.h"

int CWidget::GetOwningPlayerIndex()
{
#ifdef MDP_DEMO_PLAYER0_OWNS_WIDGETS
    //  Deliberate behaviour change - see mdp_demo.h.  Everything that
    //  reaches this slot (widget, selectable, destructable, item,
    //  CCaptainAI - but never a unit, which overrides it) now claims to
    //  belong to player 0.
    return 0;
#else
    return -1;
#endif
}
