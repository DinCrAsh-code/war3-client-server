//============================================================================
//  0x6F372C60 - CDepthCacheOwner::QueryField1C, split into its own
//  translation unit for the reason footprinttypename.cpp already gives
//  for the same shape: CDepthCacheOwner::UpdateState
//  (widget_selectiondepthcache.cpp, 0x6F372F00) calls this for real in
//  the shipped code, and MSVC inlines a same-TU four-line callee like
//  this one whenever it judges the call site profitable - which loses
//  the real, out-of-line `call` UpdateState's own dump has.  Not
//  including tshash.inl here is what keeps `m_table.Ptr(...)` below a
//  real call into the other TU's own explicit instantiation rather than
//  a second, freshly-inlined copy of the template body.
//============================================================================
#include "widget_selectiondepthcache.h"

void* CDepthCacheOwner::QueryField1C(unsigned int a0, unsigned int a1)
{
    SDepthCacheKey key(a1, a0);
    CDepthCacheRecord* record = m_table.Ptr(key.Hash(), key);
    if (record == 0)
        return 0;
    return (char*)record + 0x1C;
}
