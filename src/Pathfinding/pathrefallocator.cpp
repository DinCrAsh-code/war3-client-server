//============================================================================
//  0x6F4A0440 - SPathRefAllocator::Alloc. Was a THUNK; now a real
//  tail-call into SRegistrationContext::AllocRegistration
//  (gridregistrationalloc.cpp, 0x6F4A03F0) with the "skip unregister in
//  TeardownRegistration" bit set on the freshly allocated registration
//  (CGridRegistration::m_moveFlags, spatialgrid.h) - that
//  `or dword ptr [eax+40h], 10000000h` right on AllocRegistration's own
//  return value is what proved AllocRegistration returns a
//  CGridRegistration* in the first place (see gridregistrationalloc.cpp's
//  own header comment).
//
//  Own translation unit, deliberately NOT
//  Pathfinding/pathrefarrayresize.cpp (this function's only caller in
//  this dump, SPathRefArray::ResizeAndFillMasks): making this body visible
//  in that TU let /Ob2 inline it there and reshuffle
//  ResizeAndFillMasks's own already-EXACT register allocation - a real,
//  measured regression, not a hypothetical one. CLAUDE.md's own "put a
//  callee in its own TU rather than reaching for `__declspec(noinline)`"
//  rule, this time load-bearing for the CALLER's sake rather than this
//  function's own shape.
//============================================================================
#include "spatialgrid.h"
#include "widgetpathrefadjust.h"   // SPathRefCounted

//  Pathfinding/pathrefarrayresize.cpp declares the struct this defines a
//  member of; forward-declared identically here so this TU does not have
//  to pull in that file's own widget-specific includes.
struct SPathRefAllocator
{
    SPathRefCounted* __thiscall Alloc(void* owner, int zero);
};

SPathRefCounted* SPathRefAllocator::Alloc(void* owner, int zero)
{
    CGridRegistration* reg =
        ((SRegistrationContext*)this)->AllocRegistration(owner, zero);
    reg->m_moveFlags |= 0x10000000;
    return (SPathRefCounted*)reg;
}
