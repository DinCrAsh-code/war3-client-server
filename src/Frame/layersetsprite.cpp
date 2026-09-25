//============================================================================
//  0x6F5FAD20 - CLayer::SetSprite: swap the sprite this layer owns.
//
//  The old one is released and the field cleared *before* the new one is
//  looked at, so setting the same sprite twice still passes through a
//  released state; the shipped body does not shortcut it.  The new sprite is
//  taken with AddRefOrNull rather than AddRef - a null argument is a legal
//  "clear the sprite" and must not report ERROR_INVALID_PARAMETER - and the
//  field is only written when there is one, which is why a null argument
//  leaves the zero the release wrote.
//
//  Slot 0x84 is told either way, including the null case, and it is told the
//  argument and not the field.
//
//  The dispatch's receiver is plainly `this`, so
//  tools/vtable_dispatch_audit.py judges it; the one pushed dword is the
//  shipped call site's own (0x6F5FAD51).
//
//  The one call site in this tree is CGameUI's constructor at 0x6F2FF060,
//  with `this` = the CGameUI itself - which is evidence that CGameUI derives
//  from CLayer rather than straight from CObserver as GameUI/gameui.h has it.
//  That is another session's class and this one only reports it; the body
//  below is written against CLayer, whose module this address is in
//  (0x6F5FAxxx) and whose +0x2C field it uses.
//============================================================================
#include "clayer.h"
#include "refcnt.h"

//  Slot 0x84 (index 33) - "the sprite changed to this one".
typedef void (__thiscall *LayerSpriteChangedFn)(void* self, void* sprite);

void CLayer::SetSprite(void* sprite)
{
    if (m_field2C != 0)
    {
        ((TRefCnt*)m_field2C)->Release();
        m_field2C = 0;
    }

    if (sprite != 0)
        m_field2C = (int)((TRefCnt*)sprite)->AddRefOrNull();

    ((LayerSpriteChangedFn)(*(void***)this)[0x84 / 4])(this, sprite);
}
