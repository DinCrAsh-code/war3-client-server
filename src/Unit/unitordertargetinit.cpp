//============================================================================
//  0x6F286180 - COrderTarget::Init: COrder::Init plus the two fields a
//  targeted order adds.
//
//  Six of its eight arguments go straight through to the base (the target
//  and the last refcounted pointer are the two that do not), which is why
//  the shipped body is one long push sequence and then two short blocks:
//  the target widget chained into +0x58 the same way the base chains its
//  own source ref, and a refcounted pointer swapped into +0x64 exactly the
//  way the base swaps +0x54.
//
//  Own translation unit: COrder::Init is a real call out of it.
//============================================================================
#include "unitorder.h"
#include "agent.h"

void COrderTarget::Init(int orderId, int player, const void* sourceRef,
                        const void* target, const void* x, const void* y,
                        CAgent* owner, CAgent* tracker)
{
    m_base.Init(orderId, player, sourceRef, x, y, owner);

    ((SOptionalHandleRefResolver*)&m_target)->ResolveChained(target);

    if (tracker)
        tracker->m_refcount++;

    CAgent* old = m_pRef64;
    if (old)
    {
        if (--old->m_refcount == 0)
            old->ReleaseSelf();
    }
    m_pRef64 = tracker;
}
