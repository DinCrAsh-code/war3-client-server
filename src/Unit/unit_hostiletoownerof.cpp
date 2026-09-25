//============================================================================
//  0x6F285B00 - CUnit::IsNotRelatedToOwnerOf.
//
//  CUnit batch-10, a slot 3 / slot 72 closure leaf.  Forwards to the world
//  singleton's CGameWorld::TestRelationBitAgainstOwner (0x6F3A3430,
//  gameworldtestrelationagainstowner.cpp) with this unit's own owning
//  player (+0x58) and `widget`, then negates the 0/1 result itself
//  (`neg`/`sbb`/`add 1`, docs/msvc-vc8-idioms.md's "`!x` on a non-bool
//  `int`") - the wrapper it calls answers "is the relation bit set", this
//  one answers "is it clear".
//============================================================================
#include "unit.h"
#include "gameui.h"

extern void* g_unk6FAB65F4;

int CUnit::IsNotRelatedToOwnerOf(CWidget* widget)
{
    return !((CGameWorld*)g_unk6FAB65F4)->TestRelationBitAgainstOwner(m_owningPlayer58, widget);
}
