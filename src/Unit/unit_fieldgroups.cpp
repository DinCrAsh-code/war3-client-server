//============================================================================
//  0x6F275E20 - CUnit's vtable slot 9 (+0x24).  See unit.h.
//
//  Two halves.  First, republish the unit's own {value, min, max} triple
//  with its lower bound reset to zero and its upper bound written back
//  with the value it already had - which is FloatMiniB::SetPair
//  (floatmini.h) and is why that setter takes a *pair* rather than being
//  two one-field setters: the self-store of the field it is not changing
//  is exactly what the shipped code emits and what two independent setters
//  never would.
//
//  Then run the base's own slot 9 (0x6F2AC330, CWidget::Method_0x24 -
//  CSelectable does not override it) and hand each of the six embedded
//  sub-objects the running index it returned, one after another, each
//  through its own vtable+0x0C and each with the index one higher than the
//  last.  The final `lea eax, [esi+1]` is the same "and one more for me"
//  the base returns.
//
//  The visiting order is +0x164, +0x98, +0xB8, +0x214, +0x118, +0xFC -
//  not ascending, not declaration order, and transcribed rather than
//  tidied for the reason unit_dumpstate.cpp gives for its own.
//============================================================================
#include "unit.h"
#include "floatmini.h"
#include "CFloat.h"

//  Each sub-object's vtable slot 3 (+0x0C): (index, context), two stack
//  arguments and `this` in ecx.  The argument count is what makes this
//  worth a typedef rather than a guess - see tools/vtable_dispatch_audit.py.
typedef void (__thiscall *GroupVisitFn)(void* self, int index, int context);

int CUnit::Method_0x24(int context)
{
    FloatMiniB* bounds = (FloatMiniB*)m_fltB204;
    bounds->SetPair(g_CFloatZero, m_bound210);

    int index = CWidget::Method_0x24(context);

    ((GroupVisitFn)(*(void***)&m_movement)[0x0C / 4])(&m_movement, index, context);
    ++index;
    ((GroupVisitFn)(*(void***)&m_life)[0x0C / 4])(&m_life, index, context);
    ++index;
    ((GroupVisitFn)(*(void***)&m_mana)[0x0C / 4])(&m_mana, index, context);
    ++index;
    ((GroupVisitFn)(*(void***)&m_ref214)[0x0C / 4])(&m_ref214, index, context);
    ++index;
    ((GroupVisitFn)(*(void***)&m_ref118)[0x0C / 4])(&m_ref118, index, context);
    ++index;
    ((GroupVisitFn)(*(void***)&m_refFC)[0x0C / 4])(&m_refFC, index, context);
    return index + 1;
}
