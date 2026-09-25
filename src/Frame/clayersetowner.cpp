//============================================================================
//  0x6F5FF6D0 - CLayer::SetOwner: re-parent a layer, or make it a root.
//
//  The whole of the layer tree's bookkeeping in one function:
//
//    * optionally take the layer out of its old owner's children;
//    * swap the owner reference (addref the new one *before* releasing the
//      old, so re-setting the same owner cannot drop the last reference);
//    * push the new owner's enabled state down the subtree;
//    * with an owner, hang the layer off it, cache "my owner is visible" in
//      bit 1 of +0xB0, and mark the layer's own cache bit 0x40 set or clear
//      depending on whether the owner or any of *its* ancestors is hidden
//      (flag 8);
//    * with no owner, optionally become a root.
//
//  Every path ends by zeroing flt_6FACE660.  The two flag arms are not
//  symmetric and that is the shipped code, not a slip: the hidden arm ORs
//  (m_flagMask | 0x40) into m_flags and leaves everything else alone, while
//  the visible arm rebuilds m_flags as (m_flags & ~0x40) | m_flagMask.
//
//  Own translation unit: five real calls out of it, one to each of the other
//  five CLayer bodies.
//============================================================================
#include "clayer.h"

void __fastcall LinkLayerToRoot(CLayer* layer);   // 0x6F5FF5F0

void CLayer::SetOwner(CLayer* owner, int addToRootWhenNone, int detachFromOld)
{
    if (detachFromOld != 0 && m_pOwner != 0)
        m_pOwner->RemoveChild(this);

    if (owner != 0)
        owner->m_refcount++;

    CLayer* old = m_pOwner;
    if (old != 0)
    {
        old->m_refcount += -1;
        if (old->m_refcount == 0)
            ((LayerReleaseFn)(*(void***)old)[0])(old);
    }

    m_pOwner = owner;

    if (owner != 0)
        PropagateEnabled(owner->m_enabled, 0);

    CLayer* parent = m_pOwner;

    if (parent == 0)
    {
        if (addToRootWhenNone != 0)
            LinkLayerToRoot(this);

        g_layerElapsedTime = 0.0f;
        return;
    }

    parent->AddChild(this);

    //  Re-read: AddChild can be looking at a different owner by the time it
    //  returns, and the shipped code loads +0x20 again here.
    parent = m_pOwner;

    //  Bits 0 and 1 of the owner's +0xB0 together mean "not showing".  The
    //  0/1 goes through its own local because the shipped code materialises
    //  it (`mov eax,1` / `jmp` / `xor eax,eax`) before folding it into the
    //  bitfield write.
    int shown = ((parent->m_stateBits & 3) != 0) ? 0 : 1;
    m_state.m_ownerVisible = (shown == 0);

    if (parent != 0
        && ((parent->m_flags & 8) != 0 || parent->AncestorsHaveFlags(8)))
    {
        unsigned int hidden = m_flagMask | 0x40;
        g_layerElapsedTime = 0.0f;
        m_flags |= hidden;
        return;
    }

    unsigned int cleared = m_flags & ~0x40u;
    g_layerElapsedTime = 0.0f;
    m_flags = cleared | m_flagMask;
}
