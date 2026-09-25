//============================================================================
//  0x6F336DE0 - release a TRefCnt-derived field and clear it.  `retn 0`,
//  thiscall, no other argument.
//
//  Two callers in CItem-batch-3's own dump: sub_6F341BB0 (outside this
//  batch, not reconstructed) and CCinematicFilter::~CCinematicFilter
//  (GameUI/cinematicfilterdtor.cpp), which calls it on itself as its very
//  first statement to release +0x1C - a field GameUI/cinematicfilter.h
//  had recorded as a plain `int` (nothing in that constructor writes it a
//  refcounted pointer), refined here by this callee's own body into a
//  TRefCnt-derived handle the destructor releases.
//
//  Written against a minimal local receiver rather than any one caller's
//  full class - the shipped body only ever touches the one field, so this
//  is the generic "release and clear +0x1C" leaf every such receiver
//  reinterprets through, the same convention
//  Misc/cunit_agent6_leaves.cpp's own ReleaseAndClearField20 uses for the
//  same shape at a different offset.
//============================================================================
#include "refcnt.h"

struct SField1CRefCntHost
{
    char  m_reserved00[0x1C];
    void* m_field1C;
};

void __fastcall ReleaseAndClearField1C(SField1CRefCntHost* self)
{
    if (self->m_field1C)
    {
        ((TRefCnt*)self->m_field1C)->Release();
        self->m_field1C = 0;
    }
}
