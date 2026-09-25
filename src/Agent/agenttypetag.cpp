//============================================================================
//  Two small standalone leaves in the CAgentBaseAbs neighbourhood, reached
//  only from siblings outside this session's own closure
//  (0x6F2D0450/0x6F2D9AD0/0x6F2DA590 for the first, 0x6F2E0B70/0x6F2E3320
//  for the second) - jscript-bin1 (the union-of-callers-of-the-JASS-script-
//  loader closure).
//============================================================================

//----------------------------------------------------------------------------
//  0x6F2DC660 - `retn 0`, no `ecx` read at all.  Returns the fixed 32-bit
//  tag `'AItr'` (0x41497472 = 'A'<<24 | 'I'<<16 | 't'<<8 | 'r', MSVC's own
//  big-endian packing for a multi-character constant) - almost certainly a
//  per-class RTTI-lite type tag getter, the way this binary's "Agile"
//  object family (tsarray.inl's own `AGILE_TYPE_DATA::AGILE_TYPE_RELATIONSHIP`
//  default tag) identifies its own kinds without real RTTI (off in this
//  build - docs/msvc-vc8-idioms.md).  Not declared through any vtable here,
//  per this session's own "no virtual/vtable methods" rule - just the
//  plain function this address is.
//----------------------------------------------------------------------------
unsigned int GetAItrTypeTag()
{
    return 0x41497472u;
}

//----------------------------------------------------------------------------
//  0x6F2DC6C0 - `retn 4`, `__thiscall` with one stack argument.  A thin
//  forwarder that posts through the `CEventAnnouncer` embedded at
//  `this+0x44` (the same real broadcast helper, 0x6F477A80, that
//  src/Jass/jasstimerstartelapsed.cpp's own SJassTimer::Start posts
//  through at its own object's +0x24 - shared via Misc/eventannouncer.h
//  rather than re-declared as a second thunk to the same address). The
//  five arguments read off the dump's own stack pushes: `arg0` (this
//  function's own stack argument), a fixed `0xD01BF` event id, the
//  *unadjusted* `self` (a genuine third value riding along separately
//  from the `this+0x44` the call actually dispatches on), and two zero
//  fields `PostEvent`'s own signature carries but this call site never
//  varies. `edxArg` is read into a register but never forwarded - the
//  callee's real signature (thiscall, no edx parameter) has nowhere for
//  it to go.
//----------------------------------------------------------------------------
#include "eventannouncer.h"

void* __fastcall Agent_ForwardToOffset44(void* self, void* edxArg, void* arg0)
{
    (void)edxArg;
    return ((CEventAnnouncer*)((char*)self + 0x44))->PostEvent(
        (const int*)arg0, 0xD01BFu, self, 0, 0);
}
