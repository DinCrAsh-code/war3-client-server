//============================================================================
//  0x6F2AB310 - CWidget::NotifyTargetLost.
//
//  Not reconstructed this session: it builds a CEventTargetLost record on
//  the stack (the shipped code stamps `??_7CEventTargetLost@@6B@` into it),
//  fills in two ids - asking slot 59 (+0xEC, CWidget::GetOwningPlayerIndex)
//  for either of them that arrives as -1 - and hands the record to this
//  object's own vtable slot 4 (+0x10, CObserver::Method_0x10,
//  observer_slots.cpp), all inside the same unreproducible
//  __except_handler4-shaped SEH frame docs/msvc-vc8-idioms.md documents for
//  every other constructor-on-the-stack in this family.  Redirected to its
//  real address rather than reconstructed, so its one call site here
//  (CItem::SetPending, item_pending.cpp) matches and the live build reaches
//  the real code.
//============================================================================
#include "widget.h"

__declspec(naked) void CWidget::NotifyTargetLost(int, int)
{
    __asm
    {
        mov     eax, 06F2AB310h
        jmp     eax
    }
}
