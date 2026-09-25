//============================================================================
//  0x6F383B90 - cunit_agent5_worklist.md, CUnit closure BFS depth 1-2.
//
//  Two instructions: `mov dword ptr [ecx], offset ??_7TRefCnt@@6B@` /
//  `retn`.  This is TRefCnt::~TRefCnt's own trivial body (refcnt.h - the
//  destructor is `{}`, so all a compiled-out call to it ever does is the
//  compiler-inserted vtable-pointer stamp back to TRefCnt's own table,
//  the standard "unwind to my own class before running my body" step
//  every virtual-bearing destructor gets).  refcnt_deleteself.cpp already
//  documents this exact shape at 0x6F00C070/0x6F00C080; this image links
//  without /OPT:ICF, so a second out-of-line caller of `~TRefCnt()` earns
//  its own address rather than folding into either of those - this batch
//  has no dump for whatever calls *this* copy, only for the copy itself.
//============================================================================
#include "refcnt.h"

void __fastcall TRefCntDtorStamp_0x6F383B90(TRefCnt* self)
{
    self->TRefCnt::~TRefCnt();
}
