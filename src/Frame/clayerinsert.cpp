//============================================================================
//  0x6F5FF4B0 / 0x6F5FF5F0 - the two places a LAYERNODE is allocated.
//
//  They are the same function against two lists: a layer's own children at
//  +0x14, and the module-global root list at 0x6FA9A41C.  Both scan first
//  and do nothing if the layer is already in the list, both allocate the
//  node from the shared pool with the placement-new null test MSVC puts in
//  front of the constructor, both link it at the head, and both take a
//  reference on the layer while releasing whatever the (freshly zeroed)
//  node was holding.
//
//  AddChild does two extra things: it keeps the children sorted on +0xA8 by
//  remembering the first node that sorts after the newcomer and re-splicing
//  in front of it afterwards, and it pulls the layer out of the root list,
//  because a layer with an owner is not a root.  The re-splice is a second
//  LinkAfter through the same Link(node) expression, which is why the null
//  join appears twice.
//
//  Neither calls the other, so they share a translation unit;
//  UnlinkLayerFromRoot is in clayerchildren.cpp because AddChild calls it
//  for real.
//============================================================================
#include "clayer.h"

void __fastcall UnlinkLayerFromRoot(CLayer* layer);   // 0x6F5FDD00

//----------------------------------------------------------------------------
//  0x6F5FF4B0
//----------------------------------------------------------------------------
void CLayer::AddChild(CLayer* child)
{
    LAYERNODE* insertBefore = 0;

    int link = (int)m_children.Tail();
    while (link > 0)
    {
        LAYERNODE* node = (LAYERNODE*)link;
        CLayer* held = node->m_pLayer;

        if (held == child)
            return;

        if (insertBefore == 0 && held->m_order > child->m_order)
            insertBefore = node;

        link = (int)m_children.Prev(node);
    }

    LAYERNODE* fresh = (LAYERNODE*)
        g_layerNodeAllocator.Alloc(1, g_layerNodeTypeName, -2);
    new (fresh) LAYERNODE();

    m_children.LinkToHead(fresh);

    if (child != 0)
        child->m_refcount++;

    CLayer* old = fresh->m_pLayer;
    if (old != 0)
    {
        old->m_refcount += -1;
        if (old->m_refcount == 0)
            ((LayerReleaseFn)(*(void***)old)[0])(old);
    }

    fresh->m_pLayer = child;

    UnlinkLayerFromRoot(child);

    if (insertBefore != 0)
        m_children.LinkAfter(&insertBefore->m_link, fresh);
}

//----------------------------------------------------------------------------
//  0x6F5FF5F0 - the root-list twin.  A free `__fastcall`: the layer arrives
//  in ecx and the list it goes onto is a module global, so there is no
//  receiver.
//----------------------------------------------------------------------------
void __fastcall LinkLayerToRoot(CLayer* layer)
{
    int tail = g_layerRootPrevLink;
    int link = tail > 0 ? tail : 0;

    while (link > 0)
    {
        LAYERNODE* node = (LAYERNODE*)link;

        if (node->m_pLayer == layer)
            return;

        link = node->m_link.m_prevlink;
    }

    LAYERNODE* fresh = (LAYERNODE*)
        g_layerNodeAllocator.Alloc(1, g_layerNodeTypeName, -2);
    new (fresh) LAYERNODE();

    //  The root list's terminator is the two globals themselves; Link(0)
    //  on a real TSList would hand back exactly this address.
    TSLink<LAYERNODE>* nodeLink = fresh != 0
        ? &fresh->m_link
        : (TSLink<LAYERNODE>*)&g_layerRootNext;

    nodeLink->Unlink();

    TSLink<LAYERNODE>* head = g_layerRootNext;
    nodeLink->m_next = head;
    nodeLink->m_prevlink = head->m_prevlink;
    head->m_prevlink = (int)fresh;
    g_layerRootNext = nodeLink;

    if (layer != 0)
        layer->m_refcount++;

    CLayer* old = fresh->m_pLayer;
    if (old != 0)
    {
        old->m_refcount += -1;
        if (old->m_refcount == 0)
            ((LayerReleaseFn)(*(void***)old)[0])(old);
    }

    fresh->m_pLayer = layer;
}
