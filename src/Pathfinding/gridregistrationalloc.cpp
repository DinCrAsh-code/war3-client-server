//============================================================================
//  0x6F4A03F0 - SRegistrationContext::AllocRegistration. Reached from two
//  places: NIpse::CPoPos::Apply (Agent/popos_apply_slot4.cpp, its own
//  +0x94/+0x98 registration handles) and SPathRefAllocator::Alloc
//  (Pathfinding/pathrefallocator.cpp) - the second call site is what let
//  this session finally re-derive this, where the previous session
//  (docs/targets/NTempestNIpse_clusterH_remainder.md) had only the first
//  and could not confidently resolve it alone. `SPathRefAllocator::Alloc`'s
//  own body (`or dword ptr [eax+40h], 10000000h` right on this function's
//  own return value) is what proves the return type is a
//  CGridRegistration*: that is exactly CGridRegistration::
//  TeardownRegistration's own "already torn down / was never really
//  added" gate bit at m_moveFlags (spatialgrid.h).
//
//  AllocRegistration itself does not own any storage - it hands that off
//  to AllocatePooledRegistration (Pathfinding/pmregionpoolalloc.cpp,
//  0x6F48DA90), a generic "pop-or-grow one element from a fixed pool,
//  optionally calling the pooled object's own vtable slot 3" helper that
//  pulls from NIpse::CPmRegion's own static pool (PmRegion_GetStaticPool,
//  Agent/agentbasechain_staticpools.h, real). Its own free-list-pop-or-grow
//  step (0x6F48D410, in turn calling 0x6F48C000) stays a THUNK - see
//  pmregionpoolalloc.cpp's own comment for why - so the pooled object that
//  comes back is a REAL, shipped `??_7CPmRegion@NIpse@@6B@`-vtabled
//  instance (fresh) or an as-was reused one; either way, this
//  reconstruction only ever reads/writes the fixed CGridRegistration-shaped
//  fields at +0x1C..+0x40 on whatever pointer the pool hands back, which
//  is exactly what the shipped 0x6F4A03F0/0x6F48DA90 pair does too, so
//  nothing here depends on resolving whether the pooled object is
//  "really" a CPmRegion or a CGridRegistration to be correct.
//
//  A genuinely new finding this session made tracing the SECOND call
//  site's own register flow (not carried over from the previous session's
//  own doc, which had swapped which field gets the register argument and
//  which gets the first stack argument - trust the raw bytes over any
//  earlier paraphrase, per this repo's own standing rule): the REGISTER
//  argument (`ecx` at the call site, "this"-shaped but not really a
//  member's `this`) becomes m_grid; the FIRST stack argument becomes
//  m_nodeRecord; only the SECOND stack argument is genuinely dead for
//  AllocRegistration's own field-stamping purposes (it is still forwarded
//  into the pool's own conditional InitGrid call as `initArg`, so "dead"
//  only means "never stored on the returned object itself").
//
//  Own translation unit: called from two different modules' worth of
//  callers (Agent/ and Pathfinding/), and its own callee
//  (AllocatePooledRegistration) is a distinct out-of-line function in the
//  real dump too - the shipped body here has a real `call`, not an
//  inlined copy, which only reproduces when that callee's own definition
//  lives in a different TU (CLAUDE.md's own "put a callee in its own TU
//  rather than reaching for `__declspec(noinline)`" rule). The same rule
//  is why SPathRefAllocator::Alloc (this function's OTHER caller) also
//  needs to stay out of this TU and out of SPathRefArray::
//  ResizeAndFillMasks's own TU (pathrefarrayresize.cpp) - see
//  pathrefallocator.cpp's own header comment for the regression that
//  making it visible there caused.
//============================================================================
#include "spatialgrid.h"

//  Pathfinding/pmregionpoolalloc.cpp, 0x6F48DA90 - kept in its own TU (see
//  this file's own header comment for why that's load-bearing here).
void __fastcall AllocatePooledRegistration(void** outSlot, void* initArg,
                                           int callInit);

CGridRegistration* SRegistrationContext::AllocRegistration(void* nodeRecord,
                                                            int initArg)
{
    void* obj;
    AllocatePooledRegistration(&obj, (void*)initArg, 1);

    CGridRegistration* reg = (CGridRegistration*)obj;
    reg->m_box.m_y0 = -1;
    reg->m_box.m_x0 = -1;
    reg->m_box.m_y1 = -1;
    reg->m_box.m_x1 = -1;
    reg->m_grid       = (CCellGrid*)this;
    reg->m_nodeRecord = nodeRecord;
    reg->m_typeMask   = 0;
    reg->m_visitStamp = 0;      //  literal 0 - NOT the "-1 means never" a
                                 //  later re-test uses; this constructor
                                 //  writes 0 regardless.
    reg->m_cellCount  = 0;
    reg->m_moveFlags  = 0;

    return reg;
}
