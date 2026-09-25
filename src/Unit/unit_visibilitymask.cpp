//============================================================================
//  The three leaves CUnit::IsVisibilityMaskLive (unit_visibility.cpp,
//  0x6F28C510) stands on, all three of them redirect thunks in
//  unreconstructed_thunks.cpp until now:
//
//      0x6F26DE50  CUnit::QueryVisibleImpl          the flag gate
//      0x6F2834E0  UnitVisibilityMaskOwner_6F2834E0 whose mask it is
//      0x6F40B1E0  VisibilityMaskWord_6F40B1E0      the mask word itself
//
//  One translation unit, because the three are one module in the shipped
//  build in the sense that matters here: none of them calls another, so
//  nothing can be inlined away, and each is small enough that a file apiece
//  would be noise.
//============================================================================
#include "unit.h"

//----------------------------------------------------------------------------
//  0x6F26DE50 - the shared body behind CUnit's slot 60 (QueryVisible) and
//  the two `QueryVisibleImpl(1)` call sites in unit_thunks.cpp.
//
//  One flag word, three bits of it.  Bit 0x10000 has to be set or the
//  answer is no.  Past that the answer is yes unless *all three* of "the
//  caller passed something non-zero", "bit 0x80 is set" and "bit 0x8000000
//  is clear" hold at once.
//
//  **One short-circuiting condition, not a chain of guards.**  Spelled as
//  `if ((flags & 0x10000) == 0) return 0;` followed by a second guard, this
//  compiler emits the `xor eax, eax` / `ret 4` block *between* the two
//  tests and jumps backwards into it from the second - four instructions
//  out of place for a 13-instruction function.  Written as one `&&` chain
//  ending in `return 1`, every failing test is a forward branch to the
//  single trailing zero return, which is exactly the shipped layout.
//
//  `flags & 0x80` and not `(char)flags < 0`: MSVC already narrows the test
//  of a byte-sized mask on a dword to `test al, al` / `jns`, which is what
//  the shipped code has.  Spelling the sign test by hand costs a movsx.
//----------------------------------------------------------------------------
int CUnit::QueryVisibleImpl(int which)
{
    unsigned int flags = m_flags5C;
    if ((flags & 0x10000) != 0 &&
        (which == 0 || (flags & 0x80) == 0 || (flags & 0x8000000) != 0))
        return 1;
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F2834E0 - which object holds this unit's visibility mask.
//
//  The player-slot table's own array, indexed by the unit's owning player.
//  A null table answers null, which is why the caller's `mov ax, [ecx+328h]`
//  is guarded by nothing: the shipped code would fault the same way, and
//  the gate that stops it is CUnit::QueryVisible upstream.
//
//  `IndexedArrayHolder_6F3A1650::At` is misc_field_getters.cpp's and is
//  re-declared here rather than re-thunked - a second naked body at
//  0x6F3A1650 would be two symbols for one function and would cost the
//  original its score, because the canonicaliser compares symbol names.
//
//  Declared `__fastcall` taking the unit in ecx, which is what the shipped
//  body's `mov eax, [ecx]` vtable load says and what its `retn 0` (nothing
//  on the stack) allows.  A real `virtual` call is used for slot 59 rather
//  than a hand-written `(*(void***)this)[0xEC/4]` dispatch, so
//  vtable_dispatch_audit.py has nothing new to judge.
//----------------------------------------------------------------------------
extern void* g_unk6FAB65F4;

struct IndexedArrayHolder_6F3A1650
{
    char pad[0x58];
    int  m_data[1];
    int  At(unsigned int index);
};

void* __fastcall UnitVisibilityMaskOwner_6F2834E0(void* self)
{
    IndexedArrayHolder_6F3A1650* table =
        (IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4;
    if (table == 0)
        return 0;
    return (void*)table->At(((CUnit*)self)->GetOwningPlayerIndex());
}

//----------------------------------------------------------------------------
//  0x6F40B1E0 - the 16-bit mask at +0x328 of whatever the above hands back.
//
//  A whole file's worth of struct for one field would be a lie about a
//  layout this repo has not recovered, so the offset is applied directly -
//  the same choice widget_vtable_leaves.cpp makes for its own one-field
//  reads.  `unsigned short` and not `int`: the shipped `mov ax` is the
//  narrow load, and widening it here would add a movzx the original has
//  not got.
//----------------------------------------------------------------------------
unsigned short __fastcall VisibilityMaskWord_6F40B1E0(void* self)
{
    return *(unsigned short*)((char*)self + 0x328);
}
