//============================================================================
//  0x6F3A3430 - CGameWorld::TestRelationBitAgainstOwner.
//
//  CUnit batch-10, a slot 72/slot 3 closure leaf (reached as
//  `dword_6FAB65F4->TestRelationBitAgainstOwner(this->m_owningPlayer58,
//  widget)` from CUnit::IsHostileToOwnerOf, unit_hostiletoownerof.cpp).
//  Resolves `widget`'s owning player through its own vtable slot 59
//  (+0xEC, CWidget::GetOwningPlayerIndex) and forwards to
//  CGameWorld::TestRelationBit(player, ownerIdx) - the same two-argument
//  call TestRelationBit's own other wrappers make, just with one argument
//  computed through a virtual call instead of handed in directly.
//
//  Own translation unit, one module over from TestRelationBit itself
//  (0x6F3A3400, gameuiplayerenum.cpp) for the same reason
//  TestRelationBitClear (0x6F144B50) is kept out of that file: a call
//  compiled in the same TU as TestRelationBit's own definition gets
//  inlined by /Ob2, and the shipped stream's `call sub_6F3A3400` here
//  says that did not happen in the real build either.
//============================================================================
#include "gameui.h"
#include "widget.h"

int CGameWorld::TestRelationBitAgainstOwner(unsigned int player, CWidget* widget)
{
    return TestRelationBit(player, widget->GetOwningPlayerIndex());
}
