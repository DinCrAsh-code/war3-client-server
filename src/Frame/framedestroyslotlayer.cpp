//============================================================================
//  0x6F609210 / 0x6F609270 - DestroyFrameSlot / DestroyFrameLayer: the
//  element destructor the vector-destructor iterator runs over
//  CSimpleFrame's own `m_layerRegions[5]` (SFrameSlot) and CSimpleTop's own
//  ten-entry array (SFrameLayer) respectively (frame.h).  Both types are
//  nothing but `CRegionNodeList` (`TSList<REGIONNODE,0>`), so each body is
//  exactly that base's own compiler-generated destructor: call `UnlinkAll`
//  out of line, then unlink the terminator itself inline - the same two
//  steps `TSList<T,0>::~TSList()` (Storm/storm.h) always is.
//
//  Written against the two placeholder-typed `TSList<T,0>` instantiations
//  in Frame/frameregionlistunlink.cpp (see that file's own comment) rather
//  than against `CRegionNodeList` directly, so each of these two functions
//  keeps its own distinct `UnlinkAll` call target instead of both naming
//  the one shared `TSList<REGIONNODE,0>` symbol.
//============================================================================
#include "storm.h"
#include "framethunks.h"

struct SFrameLayerListNode;
struct SFrameSlotListNode;

void __fastcall DestroyFrameSlot(void* slot)
{
    typedef TSList<SFrameSlotListNode, 0> ListT;
    ListT* list = (ListT*)slot;
    list->UnlinkAll();
    list->m_terminator.Unlink();
}

void __fastcall DestroyFrameLayer(void* layer)
{
    typedef TSList<SFrameLayerListNode, 0> ListT;
    ListT* list = (ListT*)layer;
    list->UnlinkAll();
    list->m_terminator.Unlink();
}
