//============================================================================
//  TSExplicitList<SHORTCUTREGNODE, -572662307>::UnlinkAll (0x6F5FD600).
//
//  The run-time-link-offset twin of statusentrylist.cpp's instantiation, and
//  the same body as 0x6F46B560's: one extra instruction, the `mov eax,[ecx]`
//  that fetches m_linkoffset before every Link().
//
//  The element type is the shipped binary's.  Its one caller in this closure
//  is 0x6F600430, which opens by stamping
//  `??_7?$TSHashTable@USHORTCUTREGNODE@@VHASHKEY_NONE@@@@6B@` into the
//  object it is initialising and then drains that table's own all-nodes list
//  through here.  Nothing reaches a node's fields, so it stays incomplete.
//============================================================================
struct SHORTCUTREGNODE;

#define TSARRAY_TYPENAME \
    ".?AV?$TSExplicitList@USHORTCUTREGNODE@@$0?CCCCCCCD@@@"
#include "tsarray.inl"
#include "tslist.inl"

template void TSExplicitList<SHORTCUTREGNODE>::UnlinkAll();

//  0x6F5FF380 - the bucket array's own destructor: every live bucket is a
//  TSExplicitList<SHORTCUTREGNODE>, whose destructor (UnlinkAll() plus the
//  terminator's own inlined Unlink()) is non-trivial, so ~TSGrowableArray's
//  generic loop in Containers/tsarray.inl actually runs it per element here
//  - the same shape Render/facechartable.cpp's own
//  TSGrowableArray<TSExplicitList<CHARCODEDESC>>::~TSGrowableArray is.
template TSGrowableArray<TSExplicitList<SHORTCUTREGNODE> >::~TSGrowableArray();
