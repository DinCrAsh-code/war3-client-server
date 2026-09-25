//============================================================================
//  0x6F608FB0 / 0x6F608FF0 - two more instantiations of
//  Containers/tsfixedlist.inl's `TSList<T,0>::UnlinkAll` (see
//  Containers/terrainlistunlinkall.cpp for why any element type produces
//  the identical stream: the walk only ever touches `TSLink<T>`'s own two
//  fields, never `T` itself).
//
//  Both are reached only from Frame/framedestroyslotlayer.cpp's
//  DestroyFrameSlot/DestroyFrameLayer (0x6F609210 / 0x6F609270) draining
//  CSimpleFrame's own `m_layerRegions[5]` (SFrameSlot) and CSimpleTop's own
//  ten-entry array (SFrameLayer) respectively - both are, structurally,
//  nothing but `TSList<REGIONNODE,0>` (frame.h's own CRegionNodeList), so
//  the shipped binary really did emit the same template body twice rather
//  than fold it (no /OPT:ICF - the same thing already on record for
//  TSList<CStatus::STATUSENTRY,0> vs TSList<OsNet::OUTPUT,0>,
//  Containers/statusentrylist.cpp).  Two distinct placeholder types here
//  rather than one shared `REGIONNODE` instantiation, so each shipped
//  address keeps its own C++ symbol to score against instead of the two
//  disagreeing about which single name owns both addresses.
//============================================================================
#include "storm.h"
#include "tsfixedlist.inl"

struct SFrameLayerListNode;
struct SFrameSlotListNode;

//  0x6F608FB0 - DestroyFrameLayer's own list drain.
template void TSList<SFrameLayerListNode, 0>::UnlinkAll();
//  0x6F608FF0 - DestroyFrameSlot's own list drain.
template void TSList<SFrameSlotListNode, 0>::UnlinkAll();
