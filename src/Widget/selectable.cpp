//============================================================================
//  0x6F266C30 - CSelectable::CSelectable().  See selectable.h.
//============================================================================
#include "selectable.h"
#include "widget.h"
#include "itemplacementdispatch.h"

extern const unsigned int g_unk6F932954;   // funcmap.py's DATA table

CSelectable::CSelectable()
    //  +0x20 through +0x3C all come out of the base constructors now -
    //  CAgentWar3's m_flags zero, then CWidget's own eight stores - which
    //  run before this list and land exactly where the shipped code has
    //  them, between `call sub_6F001730` and this class's own vtable
    //  stamp.  See selectable.h.
    : m_attachmentSlot1(g_unk6F932954), m_attachmentSlot2(g_unk6F932954),
      m_attachmentRefreshFlag(0), m_pSelectionVisual(0)
{
}

//----------------------------------------------------------------------------
//  0x6F266C80 - see selectable.h.  This object's own vtable[0x1A0] does
//  now have its own dump (CItem__Method_0x1A0, 0x6F2C7600 - see
//  selectable.h's own +0x44/+0x48/+0x4C comment and docs/targets/
//  CItem__Method_0x1A0.md) but is not reconstructed itself: it is a
//  visual-attachment refresh (a selection-circle decal, a walkable-Z
//  marker), not item-placement logic, and its own two helpers reuse a
//  second, differently-addressed copy of GetConfigFloat's exact body that
//  would need its own translation unit to reproduce faithfully. The call
//  shape here - `this`, no other arguments - is still exactly what it is.
//----------------------------------------------------------------------------
typedef int (__thiscall *Vtbl0x1A0Fn)(void*);

int CSelectable::Reposition(CFloat* posX, CFloat* posY, int forward, int a4,
                             int a5, int a6, int a7, int noDelta, int a10,
                             int a11)
{
    //  Qualified, not `((CWidget*)this)->MoveTo(...)`: slot 54 is virtual
    //  now, and a call through a CWidget* lvalue dispatches where the
    //  shipped code has a direct `call`.
    CWidget::MoveTo(posX, posY, forward, a4, a5, a6, a7, noDelta,
                    0, a10, a11);
    return ((Vtbl0x1A0Fn)(*(void***)this)[0x1A0 / 4])(this);
}

//----------------------------------------------------------------------------
//  0x6F26BEA0 - see selectable.h.
//----------------------------------------------------------------------------
void CWar3Image::ClearPendingAndNotify()
{
    m_flags &= ~1u;
    //  +0x28 is CWidget::m_pSprite (widget.h) - the same field, now that
    //  CSelectable derives from CWidget.  The placement sub-object this
    //  slot pokes and the sprite CWar3Image::CreateSpriteFromDescriptor puts
    //  there are one object; only the two callers' vocabulary differed.
    void* placement = (void*)m_pSprite;
    if (placement != 0)
        SetItemPlacementFlagBits(placement, 0x4000000, 1);
}
