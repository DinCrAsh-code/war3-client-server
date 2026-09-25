//============================================================================
//  CItem's vtable slots that extend a base class's rather than replacing
//  it: each calls the inherited body directly (a real `call` to the base's
//  own address, never a vtable dispatch - going through the vtable from
//  inside the override would recurse) and then adds CItem's own two
//  timers, its Position record, or both.
//
//  Own translation unit, separate from item_vtable.cpp's leaves: every
//  function here makes real out-of-line calls that have to survive, and
//  none of them calls another.
//============================================================================
#include "item.h"

//  0x6F00D9E0 - see configgates.cpp.
void __fastcall AttachmentSetVisible(unsigned int slot, int on);

//----------------------------------------------------------------------------
//  0x6F2B5C00 - slot 8.  `lea eax,[edi+1]` is the *expression* `n + 1`
//  (docs/msvc-vc8-idioms.md), not `++n`.
//----------------------------------------------------------------------------
int CItem::Method_0x20(int context)
{
    int n = CWidget::Method_0x20(context);
    m_position.RecordSlot8(n, context, 0, 0);
    return n + 1;
}

//----------------------------------------------------------------------------
//  0x6F2B6040 - slot 9.  Same shape, but through Position's *own* vtable
//  slot 3 rather than a named function: the shipped code loads
//  `[esi+64h]` (Position's vptr) and calls `[eax+0Ch]`.
//----------------------------------------------------------------------------
typedef void (__thiscall *PositionSlot3Fn)(void* self, int n, int context);

int CItem::Method_0x24(int context)
{
    int n = CWidget::Method_0x24(context);
    ((PositionSlot3Fn)(*(void***)&m_position)[0x0C / 4])(&m_position, n, context);
    return n + 1;
}

//----------------------------------------------------------------------------
//  0x6F2B5C30 - slot 13.  The last Cancel() is a tail jump.
//----------------------------------------------------------------------------
void CItem::Deactivate()
{
    CSelectable::Deactivate();
    m_timer1.Cancel();
    m_timer2.Cancel();
}

//----------------------------------------------------------------------------
//  0x6F2B5CE0 - slot 106.  The test on the notify handle is signed
//  (`jl`), matching CWidget::ReleaseHandle38's own reading of a
//  negative-means-unset index rather than a "!= -1" comparison.
//----------------------------------------------------------------------------
void CItem::ShowAttachments()
{
    CSelectable::ShowAttachments();
    if ((int)m_position.m_notifyHandle >= 0)
        AttachmentSetVisible(m_position.m_notifyHandle, 1);
}
