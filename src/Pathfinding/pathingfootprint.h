//============================================================================
//  CPathingFootprint - RTTI-confirmed (agent_worktrees/classes/0x6F94C65C.json,
//  vtable @ 0x6F94C65C, "CPathingFootprint: TRefCnt;") two-slot class: slot 0
//  is TRefCnt::ReleaseSelf, inherited unchanged (its own vtable slot target,
//  0x6F00C060, is byte-identical to TRefCnt::ReleaseSelf's own address -
//  refcnt.h); slot 1 overrides TRefCnt::DeleteSelf with 0x6F3F03E0, which
//  this batch has not reconstructed (out of this session's own scope - no
//  dump) but is still declared here as a naked thunk so this class's own
//  vtable stays the real, two-slot shape check_vtables.py and
//  ctor_vtable_audit.py both expect, rather than an incomplete one.
//
//  Field layout is read straight off the constructor/destructor dumps
//  (pathingfootprint_ctor.cpp / pathingfootprint_dtor.cpp) - see those
//  files for the reasoning behind each name.  Everything the two targets
//  never touch is left a reserved gap.
//============================================================================
#ifndef PATHINGFOOTPRINT_H
#define PATHINGFOOTPRINT_H

#include "refcnt.h"
#include "CFloat.h"

class CPathingFootprint : public TRefCnt
{
public:
    CPathingFootprint(void* shape, void* haveExtent);   // 0x6F3EC5A0, `retn 8`
    ~CPathingFootprint();                                // 0x6F3EC740

    //  vtable slot 1 (+0x04) - overrides TRefCnt::DeleteSelf.  0x6F3F03E0,
    //  out of this session's own scope (no dump) - naked thunk, not a
    //  reconstruction.  See pathingfootprint_dtor.cpp's own header note.
    virtual TRefCnt* DeleteSelf(int flags);

    unsigned int m_gridDimA;      // +0x08 - one grid dimension (width or height)
    unsigned int m_gridDimB;      // +0x0C - the other grid dimension
    CFloat       m_extentX;       // +0x10 - CFloatFromInt(dim) * g_halfCellOffset
    CFloat       m_extentY;       // +0x14
    //  +0x18..+0x23: a small embedded buffer object (sub_6F3E95F0/
    //  sub_6F3E0AE0, out of this session's own scope) - `m_cellCount` is
    //  what the destructor actually compares against 0x40 before freeing
    //  `m_cellFlags`, NOT `m_gridDimA` above (an earlier draft of this
    //  file conflated the two - the destructor's own dump reads +0x18 and
    //  +0x20, not +0x08/+0x18, and this batch's own compare.py run against
    //  the real target caught it).
    unsigned int m_cellCount;     // +0x18
    char         m_reserved1C[4]; // +0x1C
    unsigned char* m_cellFlags;   // +0x20 - one byte per cell, gridDimA*gridDimB long
    char         m_reserved24[0x40]; // +0x24..+0x63
    unsigned int m_flags64;       // +0x64 - non-zero when any cell got the "impassable-ish" bit (or 8) set
    void*        m_haveExtent;    // +0x68 - the constructor's second argument, stored verbatim
};

#endif
