//============================================================================
//  0x6F04B840 - the loop CUnit::EnsureSelectionCircle (0x6F28DCF0) calls
//  with its own `this` unchanged (`mov ecx, esi`), so the receiver here
//  really is a plain CUnit*.  For each of `this`'s own array of
//  SField20Host_6F034D10-shaped entries (count at +0x9C, the array of
//  pointers at +0xA0), resolve the entry's own handle pair (+0x0C/+0x10 -
//  the same LookupHandle(handle, typeTag) shape game.h documents, just at
//  this struct's own offsets rather than SUnitTrackedRef's +0x08/+0x0C),
//  and if the resolved object still carries the 'lga+' tag
//  (0x2B61676C, widgetagentquery.h's own SWidgetAgentQuery::m_tag literal)
//  at its own +0x0C and its own +0x20 is still zero, release and clear the
//  entry's +0x20 field via the already-reconstructed
//  ReleaseAndClearField20 (Misc/cunit_agent6_leaves.cpp, 0x6F034D10).  The
//  tag check is MSVC's usual branchless `p->tag == t ? p : 0` ternary
//  (docs/msvc-vc8-idioms.md's own "ternary compiled branchlessly" note).
//
//  Own translation unit, not folded into cunit_agent6_leaves.cpp alongside
//  ReleaseAndClearField20 itself: with both in the same TU the compiler
//  inlines the four-instruction callee straight into this loop and, once
//  it can see there is no longer an opaque call inside the loop, also
//  hoists the +0x9C count and the +0xA0 array pointer into registers
//  across iterations - neither of which the shipped code does (it
//  re-reads both every iteration, because the real call is opaque to it).
//  A real out-of-line call here reproduces both effects at once - the
//  same reasoning SFourCCHolder030::PopulateTypeInstance's own header
//  comment gives for keeping its own callee out-of-line (Misc/
//  cunit_agent8_batch1.cpp).
//
//  NOTE ON THE RECEIVER TYPE: +0x9C/+0xA0 fall *inside* unit.h's own
//  SUnitTrackedRef m_life (+0x98, a 0x10-byte vtable/refcount/handle/
//  typeTag block) - so by that layout +0x9C would be m_life.m_refcount and
//  +0xA0 would be m_life.m_handle.  A refcount does not work as a loop
//  count and a handle does not work as a pointer-to-pointer-array, so this
//  function is not actually reading m_life through those names; either
//  unit.h's own m_life boundary does not extend as far as this call tree
//  implies, or CUnit overlays a second, unestablished view onto this exact
//  byte range that nothing in this call tree settles.  Read through raw
//  offsets below rather than guessing which and silently reshaping
//  unit.h's own struct - the same caution the sibling 0x6F20AA10 worklist
//  entry (a different, unestablished receiver entirely) already flagged
//  for this same closure.
//============================================================================
#include "game.h"

//  Re-declared to match Misc/cunit_agent6_leaves.cpp's own (non-anonymous-
//  namespace) definitions exactly, so this stays a real out-of-line call
//  into that translation unit's body rather than a second, private copy.
struct SReleasableAt0x20_6F034D10;

struct SField20Host_6F034D10
{
    char                        m_reserved00[0x20];
    SReleasableAt0x20_6F034D10* m_field20;   // +0x20
};

void __fastcall ReleaseAndClearField20(SField20Host_6F034D10* self);

namespace {

struct SResolvedTaggedRef_6F04B840
{
    char         m_reserved00[0x0C];
    unsigned int m_tag;       // +0x0C - 'lga+' (0x2B61676C) for a widget-agent-query result
    char         m_reserved10[0x10];
    unsigned int m_pending;   // +0x20 - zero while still unclaimed
};

}  // namespace

void __fastcall ReleaseStaleTaggedFieldRefs_6F04B840(void* self)
{
    for (unsigned int i = 0; i < *(unsigned int*)((char*)self + 0x9C); ++i)
    {
        SField20Host_6F034D10* entry =
            (*(SField20Host_6F034D10***)((char*)self + 0xA0))[i];
        if (!entry)
            continue;

        char* entryBase = (char*)entry;
        unsigned int handle = *(unsigned int*)(entryBase + 0x0C);
        int typeTag = *(int*)(entryBase + 0x10);

        SResolvedTaggedRef_6F04B840* resolved =
            (SResolvedTaggedRef_6F04B840*)LookupHandle(handle, typeTag);
        if (!resolved)
            continue;

        resolved = (resolved->m_tag == 0x2B61676C) ? resolved : 0;
        if (!resolved)
            continue;

        if (resolved->m_pending != 0)
            continue;

        ReleaseAndClearField20(entry);
    }
}
