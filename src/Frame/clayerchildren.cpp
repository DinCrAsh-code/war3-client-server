//============================================================================
//  0x6F5FBF70 / 0x6F5FDCB0 / 0x6F5FDEC0 - the three CLayer members that walk
//  the tree without allocating anything, plus the free root-list remover
//  0x6F5FDD00 that is the twin of one of them.
//
//  None of the four calls any of the others, which is why they share a
//  translation unit; PropagateEnabled calls *itself*, which is a real
//  recursive call in the shipped stream and stays one here.
//
//  The two removers are the same loop over two different lists - a layer's
//  own children at +0x14, and the global root list at 0x6FA9A41C - and both
//  end the same way: dispatch the node's own vtable slot 0 (the scalar
//  deleting destructor every refcounted object here carries) and hand the
//  storage back to the pool.
//============================================================================
#include "clayer.h"

const char g_layerNodeTypeName[] = ".?AULAYERNODE@CLayer@@";

//----------------------------------------------------------------------------
//  0x6F5FBF70 - does any *ancestor* carry every bit of `mask`?
//
//  The walk starts at m_pOwner rather than at `this`, so the receiver's own
//  flags are the caller's business - SetOwner tests them itself before
//  asking this.
//----------------------------------------------------------------------------
int CLayer::AncestorsHaveFlags(unsigned int mask)
{
    for (CLayer* owner = m_pOwner; owner != 0; owner = owner->m_pOwner)
    {
        if ((owner->m_flags & mask) == mask)
            return 1;
    }

    return 0;
}

//----------------------------------------------------------------------------
//  0x6F5FDCB0 - drop the node naming `child` out of this layer's children.
//
//  Walks from the tail through m_prevlink and tests its sign, the shape
//  every TSList walk in this binary has.  There is no null-out of the
//  node's own m_pLayer: the destructor slot owns that.
//----------------------------------------------------------------------------
void CLayer::RemoveChild(CLayer* child)
{
    int link = (int)m_children.Tail();
    while (link > 0)
    {
        LAYERNODE* node = (LAYERNODE*)link;

        if (node->m_pLayer == child)
        {
            ((LayerNodeDeleteFn)(*(void***)node)[0])(node, 0);
            g_layerNodeAllocator.Free(node, 0, 0);
            return;
        }

        link = (int)m_children.Prev(node);
    }
}

//----------------------------------------------------------------------------
//  0x6F5FDD00 - the same removal against the global root list.  A free
//  `__fastcall`: the layer being removed arrives in ecx and there is no
//  receiver at all, because the list it is removed from is a module global.
//----------------------------------------------------------------------------
void __fastcall UnlinkLayerFromRoot(CLayer* layer)
{
    int tail = g_layerRootPrevLink;
    int link = tail > 0 ? tail : 0;

    while (link > 0)
    {
        LAYERNODE* node = (LAYERNODE*)link;

        if (node->m_pLayer == layer)
        {
            ((LayerNodeDeleteFn)(*(void***)node)[0])(node, 0);
            g_layerNodeAllocator.Free(node, 0, 0);
            return;
        }

        link = node->m_link.m_prevlink;
    }
}

//----------------------------------------------------------------------------
//  0x6F5FDEC0 - push an enabled state down the whole subtree.
//
//  `force` is what makes it unconditional: without it a layer that already
//  holds this state stops the recursion dead, which is the cheap case when
//  a branch is re-parented under a like-for-like owner.  The state is
//  stored *before* the vtable slot 0xA4 announcement, so a handler that
//  reads it back sees the new one.
//----------------------------------------------------------------------------
void CLayer::PropagateEnabled(unsigned char enabled, int force)
{
    if (force == 0 && m_enabled == enabled)
        return;

    LayerEnabledChangedFn notify =
        (LayerEnabledChangedFn)(*(void***)this)[0xA4 / 4];
    m_enabled = enabled;
    notify(this);

    int link = (int)m_children.Tail();

    while (link > 0)
    {
        LAYERNODE* node = (LAYERNODE*)link;

        node->m_pLayer->PropagateEnabled(enabled, force);

        link = node->m_link.m_prevlink;
    }
}
