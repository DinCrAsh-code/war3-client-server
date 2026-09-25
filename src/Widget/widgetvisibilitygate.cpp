//============================================================================
//  0x6F2AC6F0 - CWidget vtable[0xF8] (index 62), and one of the three slots
//  NotifyWidgetVisibilityChanged (0x6F2AD7E0) narrows its player mask
//  through.
//
//  It takes two stack arguments and reads neither.  The answer depends only
//  on the widget: whatever vtable[0xF0] says when asked with 1, AND the
//  two flag bits 0x10 and 0x20 at CWidget+0x20 both being clear.  Bit 0x10
//  is the one CWidget::InitSprite sets or clears from its descriptor's own
//  +0x54 (widget.h), so this is the gate that flag feeds.
//
//  A thiscall member: both arguments are pushed and cleaned (`retn 8`).
//  That is what NotifyWidgetVisibilityChanged's own two-argument call site
//  says too, and reconstructing this slot is what confirms it.
//============================================================================
#include "widget.h"
#include "mdp_demo.h"

typedef int (__thiscall *QueryVisibleFn)(void*, int);

int CWidget::IsVisibilityMaskLive(unsigned int, unsigned int)
{
#ifdef MDP_DEMO_PLAYER0_OWNS_WIDGETS
    //  Deliberate behaviour change - see mdp_demo.h.
    return 1;
#else
    void* vself = this;
    if (((QueryVisibleFn)(*(void***)vself)[0xF0 / 4])(vself, 1) != 0
        && (m_flags & 0x30) == 0)
        return 1;
    return 0;
#endif
}
