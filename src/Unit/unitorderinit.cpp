//============================================================================
//  0x6F2860B0 - COrder::Init: fill a freshly made order object in.
//
//  Six values, and each one lands somewhere the rest of this path already
//  reads: the order id at +0x24 and the owning player at +0x28 (the two
//  MakeOrderAgent already writes), whatever issued the order chained into
//  +0x38, the two coordinates written through their own objects' slot 0
//  with the "tell whoever is watching" flag set, and a refcounted owner
//  swapped into +0x54.
//
//  The addref of the new owner happens *before* the release of the old one,
//  which is the shipped order and is also the only safe one when the two
//  are the same object.
//
//  Own translation unit: COrderTarget::Init calls it for real.
//============================================================================
#include "unitorder.h"
#include "agent.h"

void COrder::Init(int orderId, int player, const void* sourceRef,
                  const void* x, const void* y, CAgent* owner)
{
    m_orderId = orderId;
    m_owningPlayer = player;

    ((SOptionalHandleRefResolver*)&m_sourceRef)->ResolveChained(sourceRef);

    SetOrderCoord(&m_x, x);
    SetOrderCoord(&m_y, y);

    if (owner)
        owner->m_refcount++;

    CAgent* old = m_pOwner;
    if (old)
    {
        if (--old->m_refcount == 0)
            old->ReleaseSelf();
    }
    m_pOwner = owner;
}
