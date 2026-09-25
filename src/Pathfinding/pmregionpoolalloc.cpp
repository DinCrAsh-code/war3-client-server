//============================================================================
//  0x6F48DA90 - AllocatePooledRegistration, and its own leaf,
//  SPmRegionPoolAllocator::AllocElement (0x6F48D410, left a THUNK - see
//  its own comment below). Own translation unit, separate from
//  gridregistrationalloc.cpp's own SRegistrationContext::AllocRegistration
//  (0x6F4A03F0) even though that is this function's only caller in this
//  dump: the shipped 0x6F4A03F0 body has a real out-of-line `call` to this
//  address rather than an inlined copy, and MSVC only reproduces that when
//  the callee's definition is not visible in the caller's own TU -
//  CLAUDE.md's own "put a callee in its own TU rather than reaching for
//  `__declspec(noinline)`" rule, this time load-bearing for *this*
//  function rather than the caller.
//============================================================================
#include "spatialgrid.h"
#include "agentbasechain_staticpools.h"

//  0x6F48D410 - pops (or grows and stamps) one CPmRegion-shaped pooled
//  element from `pool` (both stack args are literal 0s at every call site
//  this session's own dump reaches - not a real "zero memory" flag pair,
//  just how this leaf's own two unused parameters happen to always be
//  called), tags it "^pma"/"^pmr" at +0xC/+0x10 and zeroes +4/+8 (on the
//  fresh-allocation path only - a reused element keeps whatever a prior
//  owner left there), then bumps the pool's own live count.  Left a
//  THUNK: its own callee, 0x6F48C000, is the free-list-pop-or-grow step
//  underneath it - a real, fully-readable function (it stamps the actual
//  `??_7CPmRegion@NIpse@@6B@` vftable on the fresh-allocation path), but
//  extending CPmRegion's own flat field layout (Agent/pmregion.h) with the
//  fields it touches is genuinely out of this session's own assigned
//  scope (CPoPos::Apply and this call chain, not CPmRegion itself) - the
//  higher-value callers above it (AllocRegistration, CPoPos::Apply,
//  SPathRefAllocator::Alloc) are real; this and 0x6F48C000 call straight
//  through to the real, unhooked shipped code either way.
struct SPmRegionPoolAllocator
{
    void* __thiscall AllocElement(int arg0, int arg4);
};

__declspec(naked) void* SPmRegionPoolAllocator::AllocElement(int, int)
{
    __asm
    {
        mov eax, 06F48D410h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F48DA90 - allocate (or reuse) one pooled element via
//  SPmRegionPoolAllocator::AllocElement above, store it through
//  `*outSlot`, and - only when `callInit` is non-zero - call the freshly
//  stored object's own vtable slot 3 (an "InitGrid"-shaped one-argument
//  method, the same slot NIpse::CPmRegion's own vtable names it at,
//  Agent/pmregion.h) with `initArg`.  `outSlot`/`initArg` arrive in
//  ecx/edx (this compiler's usual `__fastcall` spelling for a function
//  with two register-passed arguments and no real `this`), `callInit` on
//  the stack; `retn 4` cleans it.  The return value in `eax` is never read
//  by either of this function's own two call sites in this dump
//  (AllocRegistration discards it, reading the stored pointer back out of
//  `*outSlot` instead).
//----------------------------------------------------------------------------
void __fastcall AllocatePooledRegistration(void** outSlot, void* initArg,
                                           int callInit)
{
    void* obj = ((SPmRegionPoolAllocator*)PmRegion_GetStaticPool())
                    ->AllocElement(0, 0);
    *outSlot = obj;

    if (callInit != 0)
    {
        typedef void (__thiscall *InitGridFn)(void*, void*);
        InitGridFn slot3 = (InitGridFn)(*(void***)obj)[3];
        slot3(obj, initArg);
    }
}
